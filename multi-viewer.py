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

# Matrix-Ausrichtung
ROTATE_K = 2       # 0/1/2/3 (×90°). 2 = 180°
FLIP_LR  = False
FLIP_UD  = False

# RAW-Displaymodus (initial)
RAW_MODES = ["gamma", "two_stage", "percentile"]
raw_mode_idx = 1  # 0=gamma, 1=two_stage, 2=percentile

# Basis-Parameter (für RAW)
LOW_NOISE_MAX = 7000
PRESS_MAX     = 23000

# Für RAW-"two_stage"
MID_LOW      = 10000
MID_HIGH     = 19000
MID_FRACTION = 0.70

# Für RAW-"gamma"
GAMMA = 0.55

# Für RAW-"percentile"
P_LOW, P_HIGH = 2, 99
PERC_GAMMA    = 0.6

# Sentinel / Maskenwerte
INVALID_INT = -1

# ToF-Layouts
BACK_SHAPE   = (3, 2)
BACK_PLACEMENT = {0:(0,0), 1:(0,1), 2:(1,0), 3:(1,1), 4:(2,0), 5:(2,1)}
HEAD_SHAPE   = (1, 1)
KNEE_SHAPE   = (1, 2)
BOTTOM_SHAPE = (1, 2)

# ===== Delta-Skalierung: feine Änderungen sichtbar machen =====
# Δ (absolut, nur positive Erhöhungen)
DELTA_PCT    = 98     # oberes Perzentil für vmax
DELTA_MIN    = 200.0  # minimale Spannweite (Counts)
DELTA_GAIN   = 1.6    # Verstärkung

# Δ% (relativ, nur positive Erhöhungen)
RDEL_PCT     = 98     # oberes Perzentil für vmax
RDEL_MIN     = 0.02   # minimale Spannweite (2%)
RDEL_GAIN    = 2.0    # Verstärkung
RDEL_CAP     = 0.6    # harte Obergrenze für vmax (60%)

# =========================
# Normierungen (für RAW)
# =========================
def _forward_two_stage(x):
    x = np.asarray(x, dtype=float)
    y = np.empty_like(x)
    xc = np.clip(x, 0, PRESS_MAX)

    m1 = xc <= MID_LOW
    m2 = (xc > MID_LOW) & (xc <= MID_HIGH)
    m3 = xc > MID_HIGH

    low_frac = 0.15
    y[m1] = (xc[m1] / MID_LOW) * low_frac
    y[m2] = low_frac + (xc[m2]-MID_LOW) * (MID_FRACTION-low_frac) / (MID_HIGH-MID_LOW)
    y[m3] = MID_FRACTION + (xc[m3]-MID_HIGH) * (1.0-MID_FRACTION) / (PRESS_MAX-MID_HIGH)
    return y

def _inverse_two_stage(y):
    y = np.asarray(y, dtype=float)
    x = np.empty_like(y)
    low_frac = 0.15
    m1 = y <= low_frac
    m2 = (y > low_frac) & (y <= MID_FRACTION)
    m3 = y > MID_FRACTION
    x[m1] = (y[m1] / low_frac) * MID_LOW
    x[m2] = MID_LOW + (y[m2]-low_frac) * (MID_HIGH-MID_LOW) / (MID_FRACTION-low_frac)
    x[m3] = MID_HIGH + (y[m3]-MID_FRACTION) * (PRESS_MAX-MID_HIGH) / (1.0-MID_FRACTION)
    return x

def make_raw_norm(mode, data_preview=None):
    if mode == "gamma":
        return colors.PowerNorm(gamma=GAMMA, vmin=0, vmax=PRESS_MAX)
    elif mode == "two_stage":
        try:
            return colors.FuncNorm((_forward_two_stage, _inverse_two_stage), vmin=0, vmax=PRESS_MAX)
        except Exception:
            return colors.PowerNorm(gamma=GAMMA, vmin=0, vmax=PRESS_MAX)
    elif mode == "percentile":
        if data_preview is None or data_preview.size == 0:
            return colors.PowerNorm(gamma=PERC_GAMMA, vmin=0, vmax=PRESS_MAX)
        valid = data_preview[data_preview > INVALID_INT]
        if valid.size == 0:
            return colors.PowerNorm(gamma=PERC_GAMMA, vmin=0, vmax=PRESS_MAX)
        vmin = np.percentile(valid, P_LOW)
        vmax = np.percentile(valid, P_HIGH)
        if vmax <= vmin:
            vmin, vmax = 0, PRESS_MAX
        return colors.PowerNorm(gamma=PERC_GAMMA, vmin=vmin, vmax=vmax)
    else:
        return colors.Normalize(vmin=0, vmax=PRESS_MAX)

# Colormap mit Sonderfarbe für -1
base_cmap = plt.cm.plasma
CMAP = base_cmap.copy()
CMAP.set_bad('#1c1c1c')

# =========================
# Hilfen
# =========================
def orient(mat: np.ndarray) -> np.ndarray:
    m = mat
    if ROTATE_K:
        m = np.rot90(m, k=ROTATE_K)
    if FLIP_LR:
        m = np.fliplr(m)
    if FLIP_UD:
        m = np.flipud(m)
    return m

def masked_from_list(vals, shape):
    arr = np.array(vals, dtype=float).reshape(shape)
    return np.ma.masked_equal(arr, INVALID_INT)

# ===== Baseline/Delta-Steuerung =====
MODE_RAW, MODE_DELTA, MODE_RDEL = 0, 1, 2
mode = MODE_RAW
baseline = None

# Zustände für Norm/Colorbar
current_norm = None
last_used_mode = None  # "RAW" / "Δ" / "Δ%"

def force_rebuild_raw_norm():
    global current_norm, last_used_mode
    current_norm = None
    last_used_mode = None

def on_key(evt):
    global mode, baseline, raw_mode_idx
    k = (evt.key or "").lower()
    if k == 'b':
        if on_key.last_raw is not None:
            baseline = on_key.last_raw.copy()
            mn, mx = baseline.min(), baseline.max()
            print(f"✅ Baseline gespeichert (min={mn:.0f}, max={mx:.0f}). [m=Modus, r=Reset, n=RAW-Skala]")
        else:
            print("⚠️  Keine Matrix im Puffer – baseline nicht gesetzt.")
    elif k == 'r':
        baseline = None
        mode = MODE_RAW
        force_rebuild_raw_norm()
        print("↩️  Baseline gelöscht. RAW-Modus aktiv.")
    elif k == 'm':
        mode = (mode + 1) % 3   # RAW → Δ → Δ% → RAW …
        force_rebuild_raw_norm()
        print("Mode:", ["RAW", "Δ", "Δ%"][mode])
    elif k == 'n':
        if mode == MODE_RAW:
            raw_mode_idx = (raw_mode_idx + 1) % len(RAW_MODES)
            force_rebuild_raw_norm()
            print(f"RAW-Skala gewechselt: {RAW_MODES[raw_mode_idx]}")
        else:
            print("ℹ️  'n' wirkt nur im RAW-Modus.")
    elif k in ('g','t','p'):  # direkte Wahl
        if mode == MODE_RAW:
            target = {'g':"gamma",'t':"two_stage",'p':"percentile"}[k]
            raw_mode_idx = RAW_MODES.index(target)
            force_rebuild_raw_norm()
            print(f"RAW-Skala gesetzt: {target}")
        else:
            print("ℹ️  Skala direkt (g/t/p) nur im RAW-Modus umschaltbar.")

on_key.last_raw = None  # zuletzt gesehene RAW-Matrix (2D ndarray)

# =========================
# Figure / Layout
# =========================
fig = plt.figure(figsize=(11, 6))
fig.canvas.mpl_connect('key_press_event', on_key)

gs = GridSpec(nrows=2, ncols=2, width_ratios=[2.2, 1.0], height_ratios=[1.0, 1.0], figure=fig)

# Links: Matrix groß
ax_matrix = fig.add_subplot(gs[:, 0])
img_matrix = None
cbar = None

# Rechts oben: Info/Status + FSR
ax_info = fig.add_subplot(gs[0, 1]); ax_info.axis('off')

# Rechts unten: ToF
gs_tof = GridSpecFromSubplotSpec(2, 2, subplot_spec=gs[1, 1], wspace=0.25, hspace=0.35)
ax_back   = fig.add_subplot(gs_tof[0, 0])
ax_head   = fig.add_subplot(gs_tof[0, 1])
ax_knee   = fig.add_subplot(gs_tof[1, 0])
ax_bottom = fig.add_subplot(gs_tof[1, 1])
for ax in (ax_back, ax_head, ax_knee, ax_bottom):
    ax.set_xticks([]); ax.set_yticks([])
ax_matrix.set_title("Smart Chair – Matrix (JSON Stream)")

# FSR Balken
fsr_labels = ["Lordose", "Left", "Right"]
ax_fsr = ax_info.inset_axes([0.0, 0.05, 1.0, 0.45])
xpos = np.arange(len(fsr_labels))
fsr_bars = ax_fsr.bar(xpos, [0, 0, 0])
ax_fsr.set_xticks(xpos); ax_fsr.set_xticklabels(fsr_labels)
ax_fsr.set_ylim(0, max(LOW_NOISE_MAX*1.2, 25000))
ax_fsr.set_title("FSR", fontsize=10)

# Header-Text
txt_info = ax_info.text(
    0.01, 0.95, "", va='top', ha='left',
    fontsize=10, family='monospace', transform=ax_info.transAxes
)

# ToF placeholders
img_back = img_head = img_knee = img_bottom = None
for ax, title in [(ax_back,"ToF Back (3×2)"), (ax_head,"ToF Head (1×1)"),
                  (ax_knee,"ToF Knee (1×2)"), (ax_bottom,"ToF Bottom (1×2)")]:
    ax.set_title(title, fontsize=10)

# =========================
# Serial
# =========================
ser = serial.Serial(PORT, BAUD, timeout=SER_TIMEOUT)

# =========================
# Animation-Update
# =========================
def update(_frame):
    global img_matrix, cbar, img_back, img_head, img_knee, img_bottom, current_norm, last_used_mode

    line = ser.readline().decode('utf-8', 'ignore').strip()
    if not line:
        return []

    try:
        frame = json.loads(line)
    except json.JSONDecodeError:
        return []

    # ===== Telemetry-Teile ziehen =====
    t_ms     = frame.get("timestamp_ms", 0)
    posId    = frame.get("posId", -1)
    posLabel = frame.get("posLabel", "unknown")
    data = frame.get("data", {})

    # ---------- Matrix ----------
    mdata = data.get("matrix", {})
    rows  = int(mdata.get("rows", 0))
    cols  = int(mdata.get("cols", 0))
    vals  = mdata.get("values", [])

    if rows > 0 and cols > 0 and len(vals) == rows * cols:
        raw = np.array(vals, dtype=float).reshape(rows, cols)
        raw = orient(raw)
        on_key.last_raw = raw  # für 'b'

        mask = (raw == INVALID_INT)
        raw_clipped = np.where(mask, 0.0, raw)

        # ---- Modus anwenden (RAW / Δ / Δ%) ----
        used_mode = "RAW"
        show = raw_clipped.copy()
        use_delta_norm = False

        if baseline is not None and mode != MODE_RAW:
            if mode == MODE_DELTA:
                d = raw_clipped - baseline
                d[d < 0] = 0.0
                if np.count_nonzero(d > 0) >= 3:
                    show = d
                    used_mode = "Δ"
                    use_delta_norm = True
            else:  # MODE_RDEL
                base = baseline.copy()
                base[base <= 0] = 1.0
                drel = (raw_clipped - base) / base
                drel[drel < 0] = 0.0
                if np.count_nonzero(drel > 0) >= 3:
                    show = drel
                    used_mode = "Δ%"
                    use_delta_norm = True

        M_masked = np.ma.masked_array(show, mask=mask)

        # ===== Normierung wählen =====
        if use_delta_norm:
            # Empfindliche Delta-Skalierung pro Frame
            vals = M_masked.compressed()
            if used_mode == "Δ":
                hi = np.percentile(vals, DELTA_PCT) if vals.size else DELTA_MIN
                hi = max(hi, DELTA_MIN) * DELTA_GAIN
                current_norm = colors.PowerNorm(gamma=0.85, vmin=0.0, vmax=hi)
            else:  # Δ%
                hi = np.percentile(vals, RDEL_PCT) if vals.size else RDEL_MIN
                hi = min(max(hi, RDEL_MIN) * RDEL_GAIN, RDEL_CAP)
                current_norm = colors.PowerNorm(gamma=0.9, vmin=0.0, vmax=hi)
        else:
            # RAW-Skalierung (aktuellen RAW-Modus anwenden)
            raw_mode = RAW_MODES[raw_mode_idx]
            if (current_norm is None) or (last_used_mode in ("Δ", "Δ%")) or (raw_mode == "percentile"):
                # Neuaufbau erforderlich:
                dp = raw_clipped if raw_mode == "percentile" else None
                current_norm = make_raw_norm(raw_mode, data_preview=dp)

        # ===== Rendern =====
        if img_matrix is None:
            img_matrix = ax_matrix.imshow(M_masked, cmap=CMAP, norm=current_norm, origin='upper')
            # Gitter
            for x in range(M_masked.shape[1] + 1):
                ax_matrix.axvline(x - 0.5, color='white', linestyle='--', linewidth=0.5, alpha=0.5)
            for y in range(M_masked.shape[0] + 1):
                ax_matrix.axhline(y - 0.5, color='white', linestyle='--', linewidth=0.5, alpha=0.5)
            cbar = plt.colorbar(img_matrix, ax=ax_matrix, fraction=0.046, pad=0.04)
        else:
            img_matrix.set_norm(current_norm)
            img_matrix.set_data(M_masked)
            if cbar is not None:
                cbar.update_normal(img_matrix)

        # Label inkl. RAW-Submodus
        label_suffix = ""
        if used_mode == "RAW":
            label_suffix = f" · {RAW_MODES[raw_mode_idx]}"
        cbar.set_label(f"Matrix ({used_mode}{label_suffix})")

        # ---- Konsolen-Print: kompakt + midrow raw ----
        A = M_masked.filled(0.0)
        total = A.sum()
        colsN = A.shape[1]
        left  = A[:, :colsN//2].sum()
        right = A[:, colsN//2:].sum()
        lr = 0 if (left+right)==0 else (left-right)/(left+right)
        rowsN = A.shape[0]
        front = A[:rowsN//2, :].sum()
        back  = A[rowsN//2:, :].sum()
        fb = 0 if (front+back)==0 else (front-back)/(front+back)
        midrow = int(rowsN/2)
        sample = " ".join(f"{int(v):5d}" for v in raw_clipped[midrow])

        print(f"[{used_mode}] t={t_ms}ms total={total:.0f} LR={lr:+.3f} FB={fb:+.3f} | midrow raw: {sample}")

        # Optional: komplette RAW-Matrix
        print("\nMatrix:")
        for row in raw_clipped.astype(int):
            print("\t".join(f"{v:d}" for v in row))

        last_used_mode = used_mode

    # ---------- FSR ----------
    fsr = data.get("fsr", {})
    fsr_vals = fsr.get("values", [])
    if isinstance(fsr_vals, list) and len(fsr_vals) >= 3:
        for bar, v in zip(fsr_bars, fsr_vals[:3]):
            bar.set_height(0 if v == INVALID_INT else v)
        print("FSR: Lordose={}, Left={}, Right={}".format(
            *("X" if v == INVALID_INT else v for v in fsr_vals[:3])
        ))

    # ---------- ToF ----------
    tof = data.get("tof", {})

    if "back" in tof and isinstance(tof["back"], list) and len(tof["back"]) == 6:
        back = np.ma.masked_all(BACK_SHAPE, dtype=float)
        for sid, val in enumerate(tof["back"]):
            r, c = BACK_PLACEMENT[sid]
            if val == INVALID_INT: back.mask[r, c] = True
            else: back[r, c] = val; back.mask[r, c] = False
        if img_back is None:
            img_back = ax_back.imshow(back, cmap=plt.cm.inferno_r, vmin=0, vmax=1000, origin='upper')
            cmap_copy = img_back.get_cmap().copy(); cmap_copy.set_bad(color=(0.85,0.85,0.85,1.0)); img_back.set_cmap(cmap_copy)
        else:
            img_back.set_data(back)
        print("ToF Back:", ["X" if v==INVALID_INT else int(v) for v in tof["back"]])

    if "head" in tof and isinstance(tof["head"], list) and len(tof["head"]) == 1:
        head = masked_from_list(tof["head"], HEAD_SHAPE)
        if img_head is None:
            img_head = ax_head.imshow(head, cmap=plt.cm.inferno_r, vmin=0, vmax=1000, origin='upper')
            cmap_copy = img_head.get_cmap().copy(); cmap_copy.set_bad(color=(0.85,0.85,0.85,1.0)); img_head.set_cmap(cmap_copy)
        else:
            img_head.set_data(head)
        print("ToF Head:", ["X" if v==INVALID_INT else int(v) for v in tof["head"]])

    if "knee" in tof and isinstance(tof["knee"], list) and len(tof["knee"]) == 2:
        knee = masked_from_list(tof["knee"], KNEE_SHAPE)
        if img_knee is None:
            img_knee = ax_knee.imshow(knee, cmap=plt.cm.inferno_r, vmin=0, vmax=1000, origin='upper')
            cmap_copy = img_knee.get_cmap().copy(); cmap_copy.set_bad(color=(0.85,0.85,0.85,1.0)); img_knee.set_cmap(cmap_copy)
        else:
            img_knee.set_data(knee)
        print("ToF Knee:", ["X" if v==INVALID_INT else int(v) for v in tof["knee"]])

    if "bottom" in tof and isinstance(tof["bottom"], list) and len(tof["bottom"]) == 2:
        bottom = masked_from_list(tof["bottom"], BOTTOM_SHAPE)
        if img_bottom is None:
            img_bottom = ax_bottom.imshow(bottom, cmap=plt.cm.inferno_r, vmin=0, vmax=1000, origin='upper')
            cmap_copy = img_bottom.get_cmap().copy(); cmap_copy.set_bad(color=(0.85,0.85,0.85,1.0)); img_bottom.set_cmap(cmap_copy)
        else:
            img_bottom.set_data(bottom)
        print("ToF Bottom:", ["X" if v==INVALID_INT else int(v) for v in tof["bottom"]])

    # Info
    baseline_set = (baseline is not None)
    mode_str = ["RAW","Δ","Δ%"][mode]
    raw_sub = RAW_MODES[raw_mode_idx] if mode == MODE_RAW else "-"
    txt_info.set_text(
        f"time: {t_ms} ms\npos : {posLabel} ({posId})\nmode: {mode_str}"
        + (f" · {raw_sub}" if mode == MODE_RAW else "")
        + (" (baseline set)" if baseline_set else "")
    )

    return [x for x in [img_matrix, img_back, img_head, img_knee, img_bottom] if x is not None]

# =========================
# Start Animation
# =========================
ani = animation.FuncAnimation(fig, update, interval=120, blit=False, cache_frame_data=False)
plt.tight_layout()
plt.show()