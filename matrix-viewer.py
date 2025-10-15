import serial
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.colors as colors
import time

# === Konfiguration ===
PORT = '/dev/tty.SLAB_USBtoUART'   # ggf. anpassen
BAUDRATE = 115200
ROWS = 7
COLS = 7

# Orientierung (schnell justierbar)
APPLY_TRANSPOSE = True   # weil Reihen/Spalten aktuell vertauscht sind
ROTATE_K = 2             # 180°-Drehung
FLIP_X = True           # horizontal spiegeln
FLIP_Y = False           # vertikal spiegeln

# Druckbereiche
LOW_NOISE_MAX = 7000     # alles darunter = Grundrauschen
PRESS_MAX     = 23000    # Maximalwert bei starkem Druck
LOW_FRACTION  = 0.15     # Anteil der Farbskala für 0..LOW_NOISE_MAX

# === Serielle Verbindung ===
ser = serial.Serial(PORT, BAUDRATE, timeout=1)

# === Farbskala ===
base_cmap = plt.cm.plasma
cmap = base_cmap.copy()
cmap.set_bad('#1c1c1c')  # für maskierte (-1)

# === Nichtlineare Normierung ===
def _forward(x):
    x = np.asarray(x)
    y = np.empty_like(x, dtype=float)
    x_clipped = np.clip(x, 0, PRESS_MAX)
    mask1 = x_clipped <= LOW_NOISE_MAX
    y[mask1] = (x_clipped[mask1] / LOW_NOISE_MAX) * LOW_FRACTION
    mask2 = ~mask1
    y[mask2] = LOW_FRACTION + (x_clipped[mask2] - LOW_NOISE_MAX) * \
               (1.0 - LOW_FRACTION) / (PRESS_MAX - LOW_NOISE_MAX)
    return y

def _inverse(y):
    y = np.asarray(y)
    x = np.empty_like(y, dtype=float)
    y_clipped = np.clip(y, 0, 1)
    mask1 = y_clipped <= LOW_FRACTION
    x[mask1] = (y_clipped[mask1] / LOW_FRACTION) * LOW_NOISE_MAX
    mask2 = ~mask1
    x[mask2] = LOW_NOISE_MAX + (y_clipped[mask2] - LOW_FRACTION) * \
               (PRESS_MAX - LOW_NOISE_MAX) / (1.0 - LOW_FRACTION)
    return x

try:
    norm = colors.FuncNorm((_forward, _inverse), vmin=0, vmax=PRESS_MAX)
except Exception:
    norm = colors.PowerNorm(gamma=0.5, vmin=0, vmax=PRESS_MAX)

# === Plot-Setup ===
fig, ax = plt.subplots()
Z0 = np.ma.masked_equal(np.zeros((ROWS, COLS), dtype=int) - 1, -1)
heatmap = ax.imshow(Z0, cmap=cmap, norm=norm, origin='upper')

ax.set_title("Smart Chair Druckmatrix (nichtlineare Farbskala)")
ax.set_xlabel("Spalten (links → rechts)")
ax.set_ylabel("Reihen (vorn → hinten)")
ax.set_xticks(np.arange(COLS))
ax.set_yticks(np.arange(ROWS))
ax.set_xticklabels([str(i) for i in range(COLS - 1, -1, -1)])
ax.set_yticklabels([str(i) for i in range(ROWS - 1, -1, -1)])

# Gitterlinien
for x in range(COLS + 1):
    ax.axvline(x - 0.5, color='white', linestyle='--', linewidth=0.5, alpha=0.6)
for y in range(ROWS + 1):
    ax.axhline(y - 0.5, color='white', linestyle='--', linewidth=0.5, alpha=0.6)

# Colorbar
cbar = plt.colorbar(heatmap, ax=ax)
cbar.set_label("Druckwert")
cbar.set_ticks(_forward([0, LOW_NOISE_MAX, PRESS_MAX]))
cbar.set_ticklabels([f"0", f"{LOW_NOISE_MAX // 1000}k", f"{PRESS_MAX // 1000}k"])

# === Parser ===
def parse_matrix_from_serial(buffer):
    if not (buffer.startswith("[") and buffer.endswith("]")):
        return None
    clean = buffer.strip("[]")
    row_strings = clean.split(";")
    if len(row_strings) != ROWS:
        return None
    mat = np.full((ROWS, COLS), -1, dtype=int)
    for r, row_str in enumerate(row_strings):
        vals = [s.strip() for s in row_str.split(",")]
        if len(vals) != COLS:
            return None
        parsed = []
        for v in vals:
            if v.startswith('-') and v[1:].isdigit():
                parsed.append(int(v))
            elif v.isdigit():
                parsed.append(int(v))
            else:
                parsed.append(-1)
        mat[r, :] = parsed
    return mat

# === Update-Schleife ===
def update(frame):
    start_time = time.time()
    buffer = ""
    while True:
        if time.time() - start_time > 1.0:
            print("⚠️ Timeout beim Lesen. Zeige letzte gültige Matrix.")
            return [heatmap]
        try:
            part = ser.readline().decode('utf-8', errors='ignore')
        except Exception as e:
            print("SerialException:", e)
            return [heatmap]
        buffer += part.strip()
        mat = parse_matrix_from_serial(buffer)
        if mat is None:
            continue

        # Orientierung anpassen
        m = mat
        if APPLY_TRANSPOSE:
            m = m.T
        if ROTATE_K:
            m = np.rot90(m, k=ROTATE_K)
        if FLIP_X:
            m = np.fliplr(m)
        if FLIP_Y:
            m = np.flipud(m)

        rotated_masked = np.ma.masked_equal(m, -1)
        heatmap.set_data(rotated_masked)

        print("Matrix Scan:")
        for row in m:
            print('\t'.join("X" if val == -1 else str(int(val)) for val in row))
        print("----------------------")
        return [heatmap]

ani = animation.FuncAnimation(
    fig, update, interval=100, blit=False, cache_frame_data=False
)

plt.tight_layout()
plt.show()