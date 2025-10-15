import argparse
import re
import time
from typing import Dict, List, Optional
import threading
import random
import sys
import signal

import numpy as np
import serial
import serial.tools.list_ports


#To run: ~/.platformio/penv/bin/python tof-viewer.py --port /dev/tty.usbserial-110 --baud 115200

# ---- Matplotlib-Backend robust wählen ----
import matplotlib
for be in ("Qt5Agg", "MacOSX", "TkAgg", "WebAgg"):
    try:
        matplotlib.use(be)
        break
    except Exception:
        continue

# Fenster NICHT automatisch nach vorne holen
matplotlib.rcParams["figure.raise_window"] = False

import matplotlib.pyplot as plt
import matplotlib.colors as mcolors

# -------------------------------
# Konfiguration
# -------------------------------

MIN_DIST = 20
TYPICAL_MAX_DIST = 350
NO_READING = 8191  # „kein Wert“

SENSOR_GROUPS = [
    {
        "name": "Back",
        "indices": [0, 1, 2, 3, 4, 5],
        "shape": (3, 2),
        "placement": {
            0: (0, 0), 1: (0, 1),
            2: (1, 0), 3: (1, 1),
            4: (2, 0), 5: (2, 1),
        },
    },
    # Weitere Gruppen später ergänzbar ...
]

# "Sensor 3: 296 mm" (mm optional)
SENSOR_LINE_RE = re.compile(r"^\s*Sensor\s+(\d+)\s*:\s*(-?\d+)\s*(mm)?\s*$", re.IGNORECASE)
SEPARATOR_RE   = re.compile(r"^\s*-{3,}\s*$")  # „----“ (optional)

# -------------------------------
# Hilfsfunktionen
# -------------------------------

def build_group_arrays(groups_cfg: List[dict]) -> Dict[str, np.ma.MaskedArray]:
    arrays = {}
    for g in groups_cfg:
        r, c = g["shape"]
        arrays[g["name"]] = np.ma.masked_all((r, c), dtype=float)
    return arrays

def update_group_with_reading(group_cfg: dict, arr: np.ma.MaskedArray, sensor_id: int, dist_mm: int):
    if sensor_id not in group_cfg["indices"]:
        return
    r, c = group_cfg["placement"][sensor_id]
    if dist_mm >= NO_READING or dist_mm < 0:
        arr.mask[r, c] = True
    else:
        arr[r, c] = dist_mm
        arr.mask[r, c] = False

def find_serial_port_auto() -> Optional[str]:
    ports = list(serial.tools.list_ports.comports())
    if not ports:
        return None
    cu  = [p.device for p in ports if "/dev/cu."  in p.device]  # macOS bevorzugt
    tty = [p.device for p in ports if "/dev/tty." in p.device]
    if cu:  return cu[0]
    if tty: return tty[0]
    return ports[0].device

# -------------------------------
# Visualisierung
# -------------------------------

class MultiGroupHeatmap:
    def __init__(self, groups_cfg: List[dict], stop_event: threading.Event):
        self.groups_cfg = groups_cfg
        self.stop_event = stop_event
        self.arrays = build_group_arrays(groups_cfg)

        # Initialwerte mittig, damit nicht transparent
        for g in self.groups_cfg:
            arr = self.arrays[g["name"]]
            arr[:] = (MIN_DIST + TYPICAL_MAX_DIST) / 2.0
            arr.mask[:] = False

        self.fig, self.axes = plt.subplots(
            nrows=1, ncols=len(groups_cfg),
            figsize=(4.8 * len(groups_cfg), 6.0),
            constrained_layout=True,
        )
        if len(groups_cfg) == 1:
            self.axes = [self.axes]

        self.fig.patch.set_facecolor("white")
        for ax in self.axes:
            ax.set_facecolor("#f6f6f6")
            ax.set_xticks([])
            ax.set_yticks([])

        self.cmap = plt.cm.inferno_r  # nah = warm
        self.norm = mcolors.Normalize(vmin=MIN_DIST, vmax=TYPICAL_MAX_DIST)

        self.images = []
        for ax, g in zip(self.axes, groups_cfg):
            img = ax.imshow(self.arrays[g["name"]], cmap=self.cmap, norm=self.norm)
            ax.set_title(g["name"])
            cmap_copy = img.get_cmap().copy()
            cmap_copy.set_bad(color=(0.85, 0.85, 0.85, 1.0))  # maskiert = hellgrau
            img.set_cmap(cmap_copy)
            self.images.append(img)

        cbar = self.fig.colorbar(self.images[0], ax=self.axes, fraction=0.046, pad=0.04)
        cbar.set_label("Abstand (mm) – nah → warm")

        plt.ion()
        plt.show(block=False)
        plt.pause(0.05)

        # Fenster-Schließen beendet Main-Loop
        self.fig.canvas.mpl_connect("close_event", lambda evt: self.stop_event.set())

    def update_plot(self):
        for ax, g, img in zip(self.axes, self.groups_cfg, self.images):
            img.set_data(self.arrays[g["name"]])
            img.set_norm(self.norm)
        plt.pause(0.001)

# -------------------------------
# Serial-Reader (Thread)
# -------------------------------

class SerialTofReaderThread(threading.Thread):
    """
    Nicht-blockierender Serial-Reader:
    - hält zuletzt gesehene Werte (sensor_id -> dist_mm)
    - optional: nur bei „----“ Blöcke übernehmen
    """
    def __init__(self, port: str, baud: int = 115200, timeout: float = 0.1):
        super().__init__(daemon=True)
        try:
            self.ser = serial.Serial(port, baudrate=baud, timeout=timeout, exclusive=False)
        except TypeError:
            self.ser = serial.Serial(port, baudrate=baud, timeout=timeout)
        self._lock = threading.Lock()
        self._running = True
        self._latest: Dict[int, int] = {}
        self._buf_block: Dict[int, int] = {}
        self.use_blocks = False

    def run(self):
        while self._running:
            try:
                line = self.ser.readline().decode(errors="ignore").strip()
                if not line:
                    continue

                if SEPARATOR_RE.match(line):
                    if self.use_blocks:
                        with self._lock:
                            self._latest.update(self._buf_block)
                            self._buf_block.clear()
                    continue

                m = SENSOR_LINE_RE.match(line)
                if m:
                    sid = int(m.group(1)); val = int(m.group(2))
                    if self.use_blocks:
                        self._buf_block[sid] = val
                    else:
                        with self._lock:
                            self._latest[sid] = val
            except Exception:
                time.sleep(0.01)

    def get_snapshot(self) -> Dict[int, int]:
        with self._lock:
            return dict(self._latest)

    def close(self):
        self._running = False
        try:
            self.ser.close()
        except Exception:
            pass

# -------------------------------
# Main
# -------------------------------

def main():
    ap = argparse.ArgumentParser(description="Live ToF Heatmap (Serial)")
    ap.add_argument("--port", help="Serial Port (auto, wenn leer)")
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--fps", type=float, default=15.0)
    ap.add_argument("--demo", action="store_true", help="Demo-Modus mit Zufallsdaten")
    ap.add_argument("--blocks", action="store_true", help="nur bei Separator ‚----‘ updaten")
    args = ap.parse_args()

    print(f"🖼  Matplotlib-Backend: {matplotlib.get_backend()}")

    # Modus bestimmen (Demo vs. Serial)
    if args.demo:
        if args.port:
            print("ℹ️  Hinweis: --demo ignoriert --port. Es wird keine serielle Verbindung geöffnet.")
        mode = "DEMO"
        port = None
    else:
        # Ohne Argumente: Serial mit Auto-Port (kein Demo)
        if not args.port:
            auto = find_serial_port_auto()
            if not auto:
                print("❌ Kein serieller Port gefunden. Starte mit --demo oder gib --port an.", file=sys.stderr)
                sys.exit(1)
            args.port = auto
            print(f"📡 Auto-Port gewählt: {args.port}")
        mode = "SERIAL"
        port = args.port

    print(f"🚦 Modus: {mode}")
    if port:
        print(f"🔌 Port: {port} @ {args.baud} baud")

    stop_event = threading.Event()
    for sig in (signal.SIGINT, signal.SIGTERM):
        signal.signal(sig, lambda *a: stop_event.set())

    ui = MultiGroupHeatmap(SENSOR_GROUPS, stop_event)

    reader = None
    if mode == "SERIAL":
        reader = SerialTofReaderThread(port, args.baud)
        reader.use_blocks = args.blocks
        reader.start()

    try:
        last_ui = 0.0
        while not stop_event.is_set():
            if mode == "DEMO":
                snapshot = {sid: random.randint(50, 350) for sid in SENSOR_GROUPS[0]["indices"]}
                time.sleep(0.03)
            else:
                snapshot = reader.get_snapshot()

            if snapshot:
                for g in SENSOR_GROUPS:
                    arr = ui.arrays[g["name"]]
                    for sid, dist in snapshot.items():
                        if sid in g["indices"]:
                            update_group_with_reading(g, arr, sid, dist)

            now = time.time()
            if now - last_ui >= (1.0 / args.fps):
                ui.update_plot()
                last_ui = now

            plt.pause(0.001)

    finally:
        stop_event.set()
        if reader:
            reader.close()
        plt.close("all")

if __name__ == "__main__":
    main()