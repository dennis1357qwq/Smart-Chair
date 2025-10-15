import json
import time
import numpy as np
import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.colors as colors
from matplotlib.gridspec import GridSpec, GridSpecFromSubplotSpec

# =========================
# Konfiguration
# =========================
PORT = '/dev/tty.SLAB_USBtoUART'   # anpassen
BAUD = 115200
SER_TIMEOUT = 1.0

# Matrix-Ausrichtung (schnell justierbar)
ROTATE_K = 2       # 0/1/2/3 (×90°). 2 = 180°
FLIP_LR  = False   # links ↔ rechts spiegeln
FLIP_UD  = False   # oben ↔ unten spiegeln

# Matrix-Farbskala (nichtlinear, wie besprochen)
LOW_NOISE_MAX = 7000     # Grundrauschen / Preload
PRESS_MAX     = 23000    # ~ Finger-Max
LOW_FRACTION  = 0.15     # 15% der Colormap für 0..LOW_NOISE_MAX

# Sentinel / Maskenwerte
INVALID_INT = -1          # für Matrix/FSR/ToF

# ToF-Layouts (Zonen → kleine Heatmaps)
# Back: 6 Sensoren → 3x2 Raster
BACK_SHAPE   = (3, 2)
BACK_PLACEMENT = {
    0:(0,0), 1:(0,1),
    2:(1,0), 3:(1,1),
    4:(2,0), 5:(2,1),
}
# Head: 1 Sensor → 1x1
HEAD_SHAPE   = (1, 1)
# Knee: 2 Sensoren → 1x2
KNEE_SHAPE   = (1, 2)
# Bottom: 2 Sensoren → 1x2
BOTTOM_SHAPE = (1, 2)

# =========================
# Nichtlineare Normierung
# =========================
def _forward(x):
    x = np.asarray(x, dtype=float)
    y = np.empty_like(x)
    xc = np.clip(x, 0, PRESS_MAX)
    m1 = xc <= LOW_NOISE_MAX
    y[m1] = (xc[m1] / LOW_NOISE_MAX) * LOW_FRACTION
    m2 = ~m1
    y[m2] = LOW_FRACTION + (xc[m2] - LOW_NOISE_MAX) * (1.0 - LOW_FRACTION) / (PRESS_MAX - LOW_NOISE_MAX)
    return y

def _inverse(y):
    y = np.asarray(y, dtype=float)
    x = np.empty_like(y)
    yc = np.clip(y, 0, 1)
    m1 = yc <= LOW_FRACTION
    x[m1] = (yc[m1] / LOW_FRACTION) * LOW_NOISE_MAX
    m2 = ~m1
    x[m2] = LOW_NOISE_MAX + (yc[m2] - LOW_FRACTION) * (PRESS_MAX - LOW_NOISE_MAX) / (1.0 - LOW_FRACTION)
    return x

try:
    NORM = colors.FuncNorm((_forward, _inverse), vmin=0, vmax=PRESS_MAX)
except Exception:
    NORM = colors.PowerNorm(gamma=0.5, vmin=0, vmax=PRESS_MAX)

# Colormap mit Sonderfarbe für -1
base_cmap = plt.cm.plasma
CMAP = base_cmap.copy()
CMAP.set_bad('#1c1c1c')

# =========================
# Hilfen
# =========================
def orient(mat: np.ndarray) -> np.ndarray:
    """Rotation/Spiegelung anwenden."""
    m = mat
    if ROTATE_K:
        m = np.rot90(m, k=ROTATE_K)
    if FLIP_LR:
        m = np.fliplr(m)
    if FLIP_UD:
        m = np.flipud(m)
    return m

def masked_from_list(vals, shape):
    """Liste (oder len==prod(shape)) in MaskedArray der Form `shape` wandeln; INVALID_INT → maskiert."""
    arr = np.array(vals, dtype=float).reshape(shape)
    m = np.ma.masked_equal(arr, INVALID_INT)
    return m

# =========================
# Figure / Layout
# =========================
fig = plt.figure(figsize=(11, 6))
gs = GridSpec(nrows=2, ncols=2, width_ratios=[2.2, 1.0], height_ratios=[1.0, 1.0], figure=fig)

# Links: Matrix groß (nimmt beide Zeilen in Spalte 0)
ax_matrix = fig.add_subplot(gs[:, 0])
img_matrix = None
cbar = None

# Rechts oben: Info/Status + FSR
ax_info = fig.add_subplot(gs[0, 1])
ax_info.axis('off')

# Rechts unten: ToF-Zonen im 2x2 Grid
gs_tof = GridSpecFromSubplotSpec(
    nrows=2, ncols=2, subplot_spec=gs[1, 1], wspace=0.25, hspace=0.35
)

ax_back   = fig.add_subplot(gs_tof[0, 0])
ax_head   = fig.add_subplot(gs_tof[0, 1])
ax_knee   = fig.add_subplot(gs_tof[1, 0])
ax_bottom = fig.add_subplot(gs_tof[1, 1])

for ax in (ax_back, ax_head, ax_knee, ax_bottom):
    ax.set_xticks([]); ax.set_yticks([])

ax_matrix.set_title("Smart Chair – Matrix (JSON Stream)")

# FSR Balken-Plot im Info-Bereich (rechts oben)
fsr_labels = ["Lordose", "Left", "Right"]
ax_fsr = ax_info.inset_axes([0.0, 0.05, 1.0, 0.45])  # x,y,w,h (0..1 in ax_info)

xpos = np.arange(len(fsr_labels))        # -> [0,1,2]
fsr_bars = ax_fsr.bar(xpos, [0, 0, 0])   # numerische x-Werte
# Ticks + Labels setzen (kompatibel mit älteren Matplotlibs)
ax_fsr.set_xticks(xpos)
ax_fsr.set_xticklabels(fsr_labels)

ax_fsr.set_ylim(0, max(LOW_NOISE_MAX*1.2, 25000))
ax_fsr.set_title("FSR", fontsize=10)

# Header-Text (pos/timestamp)
txt_info = ax_info.text(0.01, 0.95, "", va='top', ha='left', fontsize=10, family='monospace', transform=ax_info.transAxes)

# ToF placeholders (werden beim ersten Frame erstellt)
img_back = img_head = img_knee = img_bottom = None
for ax, title in [(ax_back,"ToF Back (3×2)"),
                  (ax_head,"ToF Head (1×1)"),
                  (ax_knee,"ToF Knee (1×2)"),
                  (ax_bottom,"ToF Bottom (1×2)")]:
    ax.set_title(title, fontsize=10)

# =========================
# Serial
# =========================
ser = serial.Serial(PORT, BAUD, timeout=SER_TIMEOUT)

# =========================
# Animation-Update
# =========================
def update(_frame):
    global img_matrix, cbar, img_back, img_head, img_knee, img_bottom

    line = ser.readline().decode('utf-8', 'ignore').strip()
    if not line:
        return []

    # Versuche JSON zu parsen
    try:
        frame = json.loads(line)
    except json.JSONDecodeError:
        return []

    # ===== Telemetry-Teile ziehen =====
    t_ms     = frame.get("timestamp_ms", 0)
    posId    = frame.get("posId", -1)
    posLabel = frame.get("posLabel", "unknown")

    data = frame.get("data", {})
    # Matrix
    mdata = data.get("matrix", {})
    rows  = int(mdata.get("rows", 0))
    cols  = int(mdata.get("cols", 0))
    vals  = mdata.get("values", [])

    if rows > 0 and cols > 0 and len(vals) == rows * cols:
        M = np.array(vals, dtype=int).reshape(rows, cols)
        M = orient(M)
        M_masked = np.ma.masked_equal(M, INVALID_INT)

        if img_matrix is None:
            img_matrix = ax_matrix.imshow(M_masked, cmap=CMAP, norm=NORM, origin='upper')
            # Gitterlinien
            for x in range(M.shape[1] + 1):
                ax_matrix.axvline(x - 0.5, color='white', linestyle='--', linewidth=0.5, alpha=0.5)
            for y in range(M.shape[0] + 1):
                ax_matrix.axhline(y - 0.5, color='white', linestyle='--', linewidth=0.5, alpha=0.5)
            # Colorbar
            cbar = plt.colorbar(img_matrix, ax=ax_matrix, fraction=0.046, pad=0.04)
            cbar.set_label("Druckwert")
            cbar.set_ticks(_forward([0, LOW_NOISE_MAX, PRESS_MAX]))
            cbar.set_ticklabels([f"0", f"{LOW_NOISE_MAX//1000}k", f"{PRESS_MAX//1000}k"])
        else:
            img_matrix.set_data(M_masked)

    # FSR
    fsr = data.get("fsr", {})
    fsr_vals = fsr.get("values", [])
    if isinstance(fsr_vals, list) and len(fsr_vals) >= 3:
        for bar, v in zip(fsr_bars, fsr_vals[:3]):
            bar.set_height(0 if v == INVALID_INT else v)

    # ToF
    tof = data.get("tof", {})
    # Back
    if "back" in tof and isinstance(tof["back"], list) and len(tof["back"]) == 6:
        back = np.ma.masked_all(BACK_SHAPE, dtype=float)
        for sid, val in enumerate(tof["back"]):
            r, c = BACK_PLACEMENT[sid]
            if val == INVALID_INT:
                back.mask[r, c] = True
            else:
                back[r, c] = val
                back.mask[r, c] = False
        if img_back is None:
            img_back = ax_back.imshow(back, cmap=plt.cm.inferno_r, vmin=0, vmax=1000, origin='upper')
            # mask color hellgrau
            cmap_copy = img_back.get_cmap().copy(); cmap_copy.set_bad(color=(0.85,0.85,0.85,1.0)); img_back.set_cmap(cmap_copy)
        else:
            img_back.set_data(back)

    # Head (1)
    if "head" in tof and isinstance(tof["head"], list) and len(tof["head"]) == 1:
        head = masked_from_list(tof["head"], HEAD_SHAPE)
        if img_head is None:
            img_head = ax_head.imshow(head, cmap=plt.cm.inferno_r, vmin=0, vmax=1000, origin='upper')
            cmap_copy = img_head.get_cmap().copy(); cmap_copy.set_bad(color=(0.85,0.85,0.85,1.0)); img_head.set_cmap(cmap_copy)
        else:
            img_head.set_data(head)

    # Knee (2)
    if "knee" in tof and isinstance(tof["knee"], list) and len(tof["knee"]) == 2:
        knee = masked_from_list(tof["knee"], KNEE_SHAPE)
        if img_knee is None:
            img_knee = ax_knee.imshow(knee, cmap=plt.cm.inferno_r, vmin=0, vmax=1000, origin='upper')
            cmap_copy = img_knee.get_cmap().copy(); cmap_copy.set_bad(color=(0.85,0.85,0.85,1.0)); img_knee.set_cmap(cmap_copy)
        else:
            img_knee.set_data(knee)

    # Bottom (2)
    if "bottom" in tof and isinstance(tof["bottom"], list) and len(tof["bottom"]) == 2:
        bottom = masked_from_list(tof["bottom"], BOTTOM_SHAPE)
        if img_bottom is None:
            img_bottom = ax_bottom.imshow(bottom, cmap=plt.cm.inferno_r, vmin=0, vmax=1000, origin='upper')
            cmap_copy = img_bottom.get_cmap().copy(); cmap_copy.set_bad(color=(0.85,0.85,0.85,1.0)); img_bottom.set_cmap(cmap_copy)
        else:
            img_bottom.set_data(bottom)

    # Info-Text
    txt_info.set_text(f"time: {t_ms} ms\npos : {posLabel} ({posId})")

    return [x for x in [img_matrix, img_back, img_head, img_knee, img_bottom] if x is not None]

# =========================
# Start Animation
# =========================
ani = animation.FuncAnimation(fig, update, interval=100, blit=False, cache_frame_data=False)
plt.tight_layout()
plt.show()