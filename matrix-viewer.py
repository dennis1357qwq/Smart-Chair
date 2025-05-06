import serial
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.colors as colors
import time

# === Konfiguration ===
PORT = '/dev/cu.usbserial-0001'  # Passe ggf. an
BAUDRATE = 115200
ROWS = 10
COLS = 11

ser = serial.Serial(PORT, BAUDRATE, timeout=1)

# Farbkarte mit Sonderfarbe für -1
cmap = colors.ListedColormap(['#1c1c1c'] + plt.cm.plasma(np.linspace(0, 1, 255)).tolist())
bounds = [-2] + list(np.linspace(0, 6000, 256))
norm = colors.BoundaryNorm(bounds, cmap.N)

fig, ax = plt.subplots()
heatmap = ax.imshow(np.zeros((ROWS, COLS)), cmap=cmap, norm=norm, origin='upper')

# Achsenbeschriftungen
ax.set_title("Smart Chair Druckmatrix")
ax.set_xlabel("Spalten (links nach rechts)")
ax.set_ylabel("Reihen (vorn nach hinten)")
ax.set_xticks(np.arange(COLS))
ax.set_yticks(np.arange(ROWS))
ax.set_xticklabels([str(i) for i in range(COLS)])
ax.set_yticklabels([str(i) for i in reversed(range(ROWS))])

cbar = plt.colorbar(heatmap, ax=ax)
cbar.set_label("Druckwert")

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

        if buffer.startswith("[") and buffer.endswith("]"):
            try:
                clean = buffer.strip("[]")
                row_strings = clean.split(";")
                if len(row_strings) != COLS:
                    break

                new_matrix = np.full((COLS, ROWS), -1)
                for r, row_str in enumerate(row_strings):
                    vals = row_str.split(",")
                    if len(vals) != ROWS:
                        break
                    new_matrix[r] = [int(v.strip()) if v.strip().isdigit() else -1 for v in vals]

                rotated = np.fliplr(np.flipud(np.rot90(new_matrix, k=1)))
                heatmap.set_data(rotated)

                # Gitterlinien
                for line in ax.lines:
                    line.remove()
                for x in range(COLS + 1):
                    ax.axvline(x - 0.5, color='white', linestyle='--', linewidth=0.5)
                for y in range(ROWS + 1):
                    ax.axhline(y - 0.5, color='white', linestyle='--', linewidth=0.5)

                print("Matrix Scan:")
                for row in rotated:
                    print('\t'.join("X" if val == -1 else str(int(val)) for val in row))
                print("----------------------")
                return [heatmap]

            except Exception as e:
                print("Fehler beim Parsen:", e)
                buffer = ""
                continue

ani = animation.FuncAnimation(
    fig, update, interval=100, blit=False, cache_frame_data=False
)

plt.tight_layout()
plt.show()