import sys
import os
import time
import numpy as np
import cv2
from collections import deque
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QLabel, QPushButton,
    QVBoxLayout, QHBoxLayout, QGridLayout, QFrame, QTextEdit,
    QGroupBox, QStatusBar, QComboBox
)
from PyQt5.QtCore import Qt, QThread, pyqtSignal
from PyQt5.QtGui import (
    QImage, QPixmap, QColor, QPainter, QBrush,
    QPen, QLinearGradient
)
import serial

# =========================
# Configuration
# =========================
PORT = "COM4"
BAUD = 115200
W, H = 96, 96
FRAME_SIZE = W * H
HEADER1, HEADER2 = 0xFF, 0xAA
DISPLAY_SCALE = 6

BLACK_LEVEL = 10
GAIN = 1.5
GAMMA = 0.80

RAW_DIR = "dataset/raw"
ENH_DIR = "dataset/enh"
NORMAL_DIR = r"E:\CIMC\AI\mcu_test\normal"
DEFECT_DIR = r"E:\CIMC\AI\mcu_test\defect"

# =========================
# Industrial Color Palette
# =========================
CLR_BG_DARK      = "#0d1117"
CLR_BG_PANEL     = "#161b22"
CLR_BG_WIDGET    = "#21262d"
CLR_BORDER       = "#30363d"
CLR_TEXT_PRIMARY = "#e6edf3"
CLR_TEXT_SECONDARY = "#8b949e"
CLR_ACCENT_BLUE  = "#58a6ff"
CLR_ACCENT_GREEN = "#3fb950"
CLR_ACCENT_RED   = "#f85149"
CLR_ACCENT_AMBER = "#d2991d"
CLR_ACCENT_CYAN  = "#00d4ff"

# =========================
# Global Stylesheet
# =========================
STYLE = f"""
QMainWindow {{
    background-color: {CLR_BG_DARK};
}}
QWidget {{
    color: {CLR_TEXT_PRIMARY};
    font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
    font-size: 13px;
}}
QGroupBox {{
    border: 1px solid {CLR_BORDER};
    border-radius: 6px;
    margin-top: 18px;
    padding-top: 14px;
    font-weight: bold;
    font-size: 13px;
    color: {CLR_TEXT_SECONDARY};
}}
QGroupBox::title {{
    subcontrol-origin: margin;
    left: 14px;
    padding: 0 6px;
}}
QPushButton {{
    background-color: {CLR_BG_WIDGET};
    border: 1px solid {CLR_BORDER};
    border-radius: 5px;
    padding: 7px 16px;
    color: {CLR_TEXT_PRIMARY};
    font-weight: bold;
    font-size: 12px;
}}
QPushButton:hover {{
    background-color: #2d3642;
    border-color: {CLR_ACCENT_BLUE};
}}
QPushButton:pressed {{
    background-color: #1c232b;
}}
QPushButton#btn_normal {{
    border-color: {CLR_ACCENT_GREEN};
    color: {CLR_ACCENT_GREEN};
}}
QPushButton#btn_normal:hover {{
    background-color: #1a3a2a;
}}
QPushButton#btn_defect {{
    border-color: {CLR_ACCENT_RED};
    color: {CLR_ACCENT_RED};
}}
QPushButton#btn_defect:hover {{
    background-color: #3a1a1a;
}}
QPushButton#btn_save {{
    border-color: {CLR_ACCENT_CYAN};
    color: {CLR_ACCENT_CYAN};
}}
QPushButton#btn_save:hover {{
    background-color: #1a2e3a;
}}
QPushButton#btn_connect {{
    background-color: #1a3a2a;
    border-color: {CLR_ACCENT_GREEN};
    color: {CLR_ACCENT_GREEN};
}}
QPushButton#btn_disconnect {{
    background-color: #3a1a1a;
    border-color: {CLR_ACCENT_RED};
    color: {CLR_ACCENT_RED};
}}
QTextEdit {{
    background-color: {CLR_BG_PANEL};
    border: 1px solid {CLR_BORDER};
    border-radius: 5px;
    color: {CLR_TEXT_SECONDARY};
    font-family: "Consolas", "Cascadia Code", monospace;
    font-size: 11px;
    padding: 6px;
}}
QLabel#value_label {{
    font-family: "Consolas", "Cascadia Code", monospace;
    font-size: 22px;
    font-weight: bold;
    color: {CLR_ACCENT_CYAN};
}}
QLabel#status_label {{
    font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
    font-size: 15px;
    font-weight: bold;
}}
QComboBox {{
    background-color: {CLR_BG_WIDGET};
    border: 1px solid {CLR_BORDER};
    border-radius: 4px;
    padding: 4px 10px;
    color: {CLR_TEXT_PRIMARY};
}}
QComboBox:hover {{
    border-color: {CLR_ACCENT_BLUE};
}}
QComboBox::drop-down {{
    border: none;
    background: transparent;
}}
"""


def ensure_dirs():
    for d in [RAW_DIR, ENH_DIR, NORMAL_DIR, DEFECT_DIR]:
        os.makedirs(d, exist_ok=True)


def enhance_gray(img):
    enh = img.astype(np.float32)
    enh = (enh - BLACK_LEVEL) * GAIN
    enh = np.clip(enh, 0, 255)
    enh = 255.0 * ((enh / 255.0) ** GAMMA)
    return np.clip(enh, 0, 255).astype(np.uint8)


class SerialWorker(QThread):
    """Background thread for serial I/O."""
    frame_ready = pyqtSignal(object, object, int, int, bool, str)  # raw, enh, c0, c1, ai_ran, label
    connection_changed = pyqtSignal(bool)
    error_occurred = pyqtSignal(str)

    def __init__(self):
        super().__init__()
        self.ser = None
        self.running = False
        self.port = PORT
        self.baud = BAUD

    def connect_serial(self, port=None):
        if port:
            self.port = port
        try:
            self.ser = serial.Serial(self.port, self.baud, timeout=0.2)
            self.running = True
            self.connection_changed.emit(True)
            return True
        except serial.SerialException as e:
            self.error_occurred.emit(f"串口打开失败: {e}")
            return False

    def disconnect_serial(self):
        self.running = False
        if self.ser and self.ser.is_open:
            self.ser.close()
        self.connection_changed.emit(False)

    def read_exact(self, n, timeout_s=2.0):
        data = bytearray()
        start = time.time()
        while len(data) < n:
            chunk = self.ser.read(n - len(data))
            if chunk:
                data.extend(chunk)
            elif time.time() - start > timeout_s:
                return None
        return bytes(data)

    def run(self):
        while self.running:
            if not self.ser or not self.ser.is_open:
                time.sleep(0.1)
                continue

            b = self.ser.read(1)
            if not b or b[0] != HEADER1:
                continue

            b2 = self.ser.read(1)
            if not b2 or b2[0] != HEADER2:
                continue

            frame = self.read_exact(FRAME_SIZE)
            if frame is None:
                self.error_occurred.emit("帧超时")
                continue

            tail = self.read_exact(3, timeout_s=0.5)
            if tail is not None and tail[0] == 1:
                ai_ran = True
                class_0 = int(tail[1]) - 128
                class_1 = int(tail[2]) - 128
                diff = class_0 - class_1
                if diff >= 250:
                    ai_label = "DEFECT"
                else:
                    ai_label = "NORMAL"
            else:
                ai_ran = False
                class_0 = class_1 = 0
                diff = 0
                ai_label = "LIVE"

            raw_img = np.frombuffer(frame, dtype=np.uint8).reshape(H, W)
            enh_img = enhance_gray(raw_img)

            self.frame_ready.emit(raw_img, enh_img, class_0, class_1, ai_ran, ai_label)

    def stop(self):
        self.running = False
        if self.ser and self.ser.is_open:
            self.ser.close()


class IndicatorWidget(QWidget):
    """Custom industrial indicator light."""
    def __init__(self, color=CLR_ACCENT_GREEN, size=10):
        super().__init__()
        self.color = QColor(color)
        self.setFixedSize(size + 4, size + 4)
        self._size = size

    def set_color(self, color):
        self.color = QColor(color)
        self.update()

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        center = self.rect().center()
        # glow
        glow = QColor(self.color)
        glow.setAlpha(60)
        p.setBrush(QBrush(glow))
        p.setPen(Qt.NoPen)
        p.drawEllipse(center, self._size // 2 + 3, self._size // 2 + 3)
        # core
        grad = QLinearGradient(0, 0, self._size, self._size)
        grad.setColorAt(0, self.color.lighter(180))
        grad.setColorAt(1, self.color.darker(120))
        p.setBrush(QBrush(grad))
        p.setPen(QPen(self.color.darker(150), 1))
        p.drawEllipse(center, self._size // 2, self._size // 2)
        p.end()


class DataTile(QFrame):
    """A single data display tile with label + value."""
    def __init__(self, title, unit="", parent=None):
        super().__init__(parent)
        self.setStyleSheet(f"""
            DataTile {{
                background-color: {CLR_BG_WIDGET};
                border: 1px solid {CLR_BORDER};
                border-radius: 8px;
                padding: 10px;
            }}
        """)
        layout = QVBoxLayout(self)
        layout.setContentsMargins(12, 8, 12, 8)
        layout.setSpacing(4)

        self.title_label = QLabel(title)
        self.title_label.setStyleSheet(f"color: {CLR_TEXT_SECONDARY}; font-size: 11px; border: none; background: transparent;")
        layout.addWidget(self.title_label)

        self.value_label = QLabel("--")
        self.value_label.setObjectName("value_label")
        layout.addWidget(self.value_label)

        self.unit_label = QLabel(unit)
        self.unit_label.setStyleSheet(f"color: {CLR_TEXT_SECONDARY}; font-size: 10px; border: none; background: transparent;")
        layout.addWidget(self.unit_label)

    def set_value(self, text, color=None):
        self.value_label.setText(str(text))
        if color:
            self.value_label.setStyleSheet(
                f"font-family: 'Consolas', 'Cascadia Code', monospace; font-size: 22px; font-weight: bold; color: {color};")


class IndustrialHMI(QMainWindow):
    def __init__(self):
        super().__init__()
        ensure_dirs()

        self.setWindowTitle("BEARING INSPECTION SYSTEM  |  CIMC 2025")
        self.setMinimumSize(1050, 700)
        self.resize(1100, 750)

        self.frame_count = 0
        self.total_frames = 0
        self.t0 = time.time()
        self.fps_history = deque(maxlen=30)

        # Serial worker
        self.worker = SerialWorker()
        self.worker.frame_ready.connect(self.on_frame)
        self.worker.connection_changed.connect(self.on_connection_changed)
        self.worker.error_occurred.connect(self.on_error)
        self._connected = False

        self._setup_ui()
        self._apply_style()

    def _setup_ui(self):
        central = QWidget()
        self.setCentralWidget(central)
        root = QHBoxLayout(central)
        root.setContentsMargins(12, 12, 12, 12)
        root.setSpacing(10)

        # ==================== LEFT: Image Panel ====================
        left_panel = QVBoxLayout()
        left_panel.setSpacing(8)

        img_group = QGroupBox("LIVE FEED")
        img_layout = QVBoxLayout(img_group)
        img_layout.setContentsMargins(8, 20, 8, 8)

        self.image_label = QLabel()
        self.image_label.setAlignment(Qt.AlignCenter)
        self.image_label.setMinimumSize(580, 580)
        self.image_label.setStyleSheet(f"""
            QLabel {{
                background-color: {CLR_BG_PANEL};
                border: 1px solid {CLR_BORDER};
                border-radius: 4px;
            }}
        """)
        img_layout.addWidget(self.image_label)
        left_panel.addWidget(img_group)

        # ==================== RIGHT: Dashboard ====================
        right_widget = QWidget()
        right_widget.setMaximumWidth(320)
        right_panel = QVBoxLayout(right_widget)
        right_panel.setSpacing(8)

        # -- Connection --
        conn_group = QGroupBox("CONNECTION")
        conn_layout = QHBoxLayout(conn_group)
        conn_layout.setContentsMargins(10, 20, 10, 10)

        self.port_combo = QComboBox()
        for i in range(1, 17):
            self.port_combo.addItem(f"COM{i}")
        self.port_combo.setCurrentText(PORT)

        self.conn_indicator = IndicatorWidget(CLR_ACCENT_RED, 10)

        self.btn_connect = QPushButton("CONNECT")
        self.btn_connect.setObjectName("btn_connect")
        self.btn_connect.clicked.connect(self._toggle_connection)

        conn_layout.addWidget(self.conn_indicator)
        conn_layout.addWidget(self.port_combo)
        conn_layout.addWidget(self.btn_connect)
        right_panel.addWidget(conn_group)

        # -- Status Dashboard --
        status_group = QGroupBox("STATUS")
        status_layout = QGridLayout(status_group)
        status_layout.setContentsMargins(10, 20, 10, 10)
        status_layout.setSpacing(8)

        self.tile_fps = DataTile("FRAME RATE", "fps")
        self.tile_total = DataTile("TOTAL COUNT", "frames")
        self.tile_model = DataTile("AI RESULT", "")
        self.tile_state = DataTile("SYSTEM STATE", "")

        status_layout.addWidget(self.tile_fps, 0, 0)
        status_layout.addWidget(self.tile_total, 0, 1)
        status_layout.addWidget(self.tile_model, 1, 0, 1, 2)
        status_layout.addWidget(self.tile_state, 2, 0, 1, 2)
        right_panel.addWidget(status_group)

        # -- AI Detail --
        detail_group = QGroupBox("AI CONFIDENCE")
        detail_layout = QVBoxLayout(detail_group)
        detail_layout.setContentsMargins(10, 20, 10, 10)

        self.conf_label = QLabel("Class 0:  --\nClass 1:  --")
        self.conf_label.setStyleSheet(f"""
            font-family: 'Consolas', 'Cascadia Code', monospace;
            font-size: 12px;
            color: {CLR_TEXT_SECONDARY};
            padding: 6px;
        """)
        detail_layout.addWidget(self.conf_label)
        right_panel.addWidget(detail_group)

        # -- Controls --
        ctrl_group = QGroupBox("DATASET SAVE")
        ctrl_layout = QVBoxLayout(ctrl_group)
        ctrl_layout.setContentsMargins(10, 20, 10, 10)
        ctrl_layout.setSpacing(6)

        self.btn_save_raw = QPushButton("SAVE RAW + ENHANCED")
        self.btn_save_raw.setObjectName("btn_save")
        self.btn_save_raw.clicked.connect(lambda: self._save_frame("raw"))
        ctrl_layout.addWidget(self.btn_save_raw)

        btn_row = QHBoxLayout()
        self.btn_save_normal = QPushButton("NORMAL")
        self.btn_save_normal.setObjectName("btn_normal")
        self.btn_save_normal.clicked.connect(lambda: self._save_frame("normal"))
        btn_row.addWidget(self.btn_save_normal)

        self.btn_save_defect = QPushButton("DEFECT")
        self.btn_save_defect.setObjectName("btn_defect")
        self.btn_save_defect.clicked.connect(lambda: self._save_frame("defect"))
        btn_row.addWidget(self.btn_save_defect)
        ctrl_layout.addLayout(btn_row)
        right_panel.addWidget(ctrl_group)

        right_panel.addStretch()

        # ==================== Bottom: Log ====================
        log_group = QGroupBox("EVENT LOG")
        log_layout = QVBoxLayout(log_group)
        log_layout.setContentsMargins(8, 20, 8, 8)

        self.log_text = QTextEdit()
        self.log_text.setReadOnly(True)
        self.log_text.setMaximumHeight(110)
        log_layout.addWidget(self.log_text)

        # Compose layout
        left_area = QWidget()
        left_area.setLayout(left_panel)

        main_area = QHBoxLayout()
        main_area.addWidget(left_area, 1)
        main_area.addWidget(right_widget, 0)

        root_layout = QVBoxLayout()
        root_layout.addLayout(main_area)
        root_layout.addWidget(log_group)
        root.addLayout(root_layout)

        # Status bar
        self.status_bar = QStatusBar()
        self.status_bar.setStyleSheet(f"""
            QStatusBar {{
                background-color: {CLR_BG_PANEL};
                color: {CLR_TEXT_SECONDARY};
                border-top: 1px solid {CLR_BORDER};
                font-size: 11px;
            }}
        """)
        self.status_label = QLabel("DISCONNECTED")
        self.status_bar.addPermanentWidget(self.status_label)
        self.setStatusBar(self.status_bar)

    def _apply_style(self):
        self.setStyleSheet(STYLE)

    def _toggle_connection(self):
        if self._connected:
            self.worker.stop()
            self._connected = False
            self.btn_connect.setText("CONNECT")
            self.btn_connect.setObjectName("btn_connect")
            self.conn_indicator.set_color(CLR_ACCENT_RED)
            self.status_label.setText("DISCONNECTED")
            self._apply_style()
            self._log("系统已断开连接")
        else:
            port = self.port_combo.currentText()
            if self.worker.connect_serial(port):
                self.worker.start()
                self._connected = True
                self.btn_connect.setText("DISCONNECT")
                self.btn_connect.setObjectName("btn_disconnect")
                self.conn_indicator.set_color(CLR_ACCENT_GREEN)
                self.status_label.setText(f"CONNECTED  |  {port} @ {BAUD} bps")
                self._apply_style()
                self._log(f"系统已连接 - {port} @ {BAUD} bps")
            else:
                self._log(f"[ERROR] 无法打开 {port}")

    def on_frame(self, raw_img, enh_img, class_0, class_1, ai_ran, ai_label):
        self._raw_img = raw_img
        self._enh_img = enh_img
        self._class_0 = class_0
        self._class_1 = class_1
        self._ai_label = ai_label

        # FPS
        self.frame_count += 1
        self.total_frames += 1
        now = time.time()
        elapsed = now - self.t0
        if elapsed >= 1.0:
            fps = self.frame_count / elapsed
            self.fps_history.append(fps)
            self.frame_count = 0
            self.t0 = now
            avg_fps = sum(self.fps_history) / len(self.fps_history)
            self.tile_fps.set_value(f"{avg_fps:.1f}")
            diff = class_0 - class_1
            self._log(f"FPS={avg_fps:.1f} | AI={ai_label} | diff={diff} C0={class_0} C1={class_1}")

        self.tile_total.set_value(str(self.total_frames))

        # AI result styling (matches MCU/OLED logic)
        diff = class_0 - class_1
        if ai_ran:
            if ai_label == "DEFECT":
                self.tile_model.set_value("DEFECT", CLR_ACCENT_RED)
                self.tile_state.set_value("ALARM", CLR_ACCENT_RED)
            else:
                self.tile_model.set_value("NORMAL", CLR_ACCENT_GREEN)
                self.tile_state.set_value("PASS", CLR_ACCENT_GREEN)
        else:
            self.tile_model.set_value("LIVE", CLR_ACCENT_CYAN)
            self.tile_state.set_value("MONITORING", CLR_ACCENT_CYAN)

        self.conf_label.setText(f"diff:  {diff:>5d}\nClass 0:  {class_0:>5d}\nClass 1:  {class_1:>5d}")

        # Display enhanced image with border overlay
        show = cv2.resize(enh_img, (W * DISPLAY_SCALE, H * DISPLAY_SCALE),
                          interpolation=cv2.INTER_NEAREST)
        rgb = cv2.cvtColor(show, cv2.COLOR_GRAY2RGB)

        # AI result border
        if ai_ran:
            if ai_label == "DEFECT":
                border_color = (240, 80, 70)
            else:
                border_color = (60, 185, 80)
            cv2.rectangle(rgb, (0, 0), (rgb.shape[1] - 1, rgb.shape[0] - 1), border_color, 5)

        qimg = QImage(rgb.data, rgb.shape[1], rgb.shape[0], rgb.shape[1] * 3, QImage.Format_RGB888)
        self.image_label.setPixmap(QPixmap.fromImage(qimg.copy()))

    def on_connection_changed(self, connected):
        if not connected and self._connected:
            self._connected = False
            self.btn_connect.setText("CONNECT")
            self.btn_connect.setObjectName("btn_connect")
            self.conn_indicator.set_color(CLR_ACCENT_RED)
            self.status_label.setText("DISCONNECTED")
            self._apply_style()

    def on_error(self, msg):
        self._log(f"[ERROR] {msg}")

    def _save_frame(self, label):
        if not hasattr(self, '_enh_img'):
            self._log("[WARN] 无可用帧, 请等待图像到达")
            return

        ts = int(time.time() * 1000)

        if label == "raw":
            raw_path = os.path.join(RAW_DIR, f"raw_{ts}.png")
            enh_path = os.path.join(ENH_DIR, f"enh_{ts}.png")
            cv2.imwrite(raw_path, self._raw_img)
            cv2.imwrite(enh_path, self._enh_img)
            self._log(f"[SAVE] {raw_path}")
            self._log(f"[SAVE] {enh_path}")
        elif label == "normal":
            path = os.path.join(NORMAL_DIR, f"normal_{ts}.png")
            cv2.imwrite(path, self._enh_img)
            self._log(f"[SAVE-NORMAL] {path}")
        elif label == "defect":
            path = os.path.join(DEFECT_DIR, f"defect_{ts}.png")
            cv2.imwrite(path, self._enh_img)
            self._log(f"[SAVE-DEFECT] {path}")

    def _log(self, msg):
        ts = time.strftime("%H:%M:%S")
        self.log_text.append(f"[{ts}]  {msg}")

    def closeEvent(self, event):
        self.worker.stop()
        self.worker.wait(1000)
        event.accept()
33  

def main():
    app = QApplication(sys.argv)
    app.setApplicationName("Bearing Inspection System")
    app.setOrganizationName("CIMC")

    # Dark palette
    app.setStyle("Fusion")
    palette = app.palette()
    palette.setColor(app.palette().Window, QColor(CLR_BG_DARK))
    palette.setColor(app.palette().WindowText, QColor(CLR_TEXT_PRIMARY))
    palette.setColor(app.palette().Base, QColor(CLR_BG_PANEL))
    palette.setColor(app.palette().AlternateBase, QColor(CLR_BG_WIDGET))
    palette.setColor(app.palette().ToolTipBase, QColor(CLR_ACCENT_CYAN))
    palette.setColor(app.palette().ToolTipText, QColor(CLR_BG_DARK))
    palette.setColor(app.palette().Text, QColor(CLR_TEXT_PRIMARY))
    palette.setColor(app.palette().Button, QColor(CLR_BG_WIDGET))
    palette.setColor(app.palette().ButtonText, QColor(CLR_TEXT_PRIMARY))
    palette.setColor(app.palette().BrightText, QColor(CLR_ACCENT_RED))
    palette.setColor(app.palette().Highlight, QColor(CLR_ACCENT_BLUE))
    palette.setColor(app.palette().HighlightedText, QColor(CLR_BG_DARK))
    app.setPalette(palette)

    window = IndustrialHMI()
    window.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
