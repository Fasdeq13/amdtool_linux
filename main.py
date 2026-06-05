import sys
import os
import glob
import subprocess
from PySide6.QtWidgets import QApplication, QMainWindow, QWidget, QHBoxLayout, QVBoxLayout, QLabel, QPushButton, QScrollArea, QFrame, QGridLayout, QComboBox, QTextEdit, QLineEdit
from PySide6.QtCore import Qt, QUrl, QProcess
from PySide6.QtGui import QFont, QDesktopServices

class AdrenalinApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("AMD Software: Adrenalin Edition")
        self.resize(1200, 750)
        self.setMinimumSize(1100, 700)
        
        self.setStyleSheet("""
            QMainWindow { background-color: #101010; }
            QLabel { color: #e0e0e0; font-family: 'Segoe UI', 'Arial', sans-serif; border: none; background: transparent; }
            QPushButton { font-family: 'Segoe UI', 'Arial', sans-serif; border: none; }
        """)
        
        self.tabs = ["Home", "Gaming", "Performance", "Driver"]
        self.active_tab = "Home"
        self.process = None
        self.build_main_frame()

    def build_main_frame(self):
        self.root_widget = QWidget()
        self.setCentralWidget(self.root_widget)
        self.root_layout = QVBoxLayout(self.root_widget)
        self.root_layout.setContentsMargins(0, 0, 0, 0)
        self.root_layout.setSpacing(0)
        
        self.build_navbar()
        
        self.body_container = QWidget()
        self.body_layout = QHBoxLayout(self.body_container)
        self.body_layout.setContentsMargins(25, 25, 25, 25)
        self.body_layout.setSpacing(15)
        
        self.root_layout.addWidget(self.body_container)
        
        if self.active_tab == "Home":
            self.render_home_view()
        elif self.active_tab == "Gaming":
            self.render_gaming_view()
        elif self.active_tab == "Performance":
            self.render_performance_view()
        elif self.active_tab == "Driver":
            self.render_driver_view()

    def build_navbar(self):
        navbar = QWidget()
        navbar.setFixedHeight(45)
        navbar.setStyleSheet("background-color: #181818; border-bottom: 1px solid #282828;")
        nav_layout = QHBoxLayout(navbar)
        nav_layout.setContentsMargins(25, 0, 25, 0)
        nav_layout.setSpacing(5)
        
        logo = QLabel("🔴 AMD")
        logo.setFont(QFont("Segoe UI", 12, QFont.Bold))
        logo.setStyleSheet("color: #e11324; margin-right: 20px;")
        nav_layout.addWidget(logo)
        
        for tab_name in self.tabs:
            btn = QPushButton(tab_name)
            btn.setFlat(True)
            btn.setCursor(Qt.PointingHandCursor)
            if tab_name == self.active_tab:
                btn.setStyleSheet("color: #ffffff; font-size: 13px; font-weight: bold; border-bottom: 2px solid #e11324; padding: 12px 15px 10px 15px; background: transparent;")
            else:
                btn.setStyleSheet("color: #8e8e8e; font-size: 13px; padding: 14px; background: transparent;")
            btn.clicked.connect(lambda checked=False, t=tab_name: self.switch_tab_file(t))
            nav_layout.addWidget(btn)
            
        nav_layout.addStretch()
        self.root_layout.addWidget(navbar)

    def switch_tab_file(self, target):
        if self.process and self.process.state() == QProcess.ProcessState.Running:
            self.process.terminate()
            self.process.waitForFinished()
            
        self.active_tab = target
        if self.root_widget:
            self.root_widget.deleteLater()
        self.build_main_frame()

    def render_home_view(self):
        left_grid = QGridLayout()
        left_grid.setSpacing(15)
        left_grid.setContentsMargins(0, 0, 0, 0)
        
        last_game_box = QFrame()
        last_game_box.setStyleSheet("background-color: #181818; border: 1px solid #282828; border-radius: 4px;")
        lg_layout = QVBoxLayout(last_game_box)
        lg_layout.setContentsMargins(20, 15, 20, 15)
        
        lg_title = QLabel("LAST PLAYED")
        lg_title.setFont(QFont("Segoe UI", 8, QFont.Bold))
        lg_title.setStyleSheet("color: #8e8e8e; letter-spacing: 0.5px;")
        lg_layout.addWidget(lg_title)
        
        self.lg_name = QLabel("No Games Detected")
        self.lg_name.setFont(QFont("Segoe UI", 14, QFont.Bold))
        self.lg_name.setStyleSheet("color: white; margin-top: 5px;")
        lg_layout.addWidget(self.lg_name)
        
        stats_layout = QHBoxLayout()
        stats_layout.setSpacing(10)
        
        stat1 = QVBoxLayout()
        lbl1 = QLabel("Hours Played")
        lbl1.setStyleSheet("color: #8e8e8e; font-size: 11px;")
        self.val1 = QLabel("0")
        self.val1.setFont(QFont("Segoe UI", 16, QFont.Bold))
        stat1.addWidget(lbl1)
        stat1.addWidget(self.val1)
        
        stat2 = QVBoxLayout()
        lbl2 = QLabel("Average FPS")
        lbl2.setStyleSheet("color: #8e8e8e; font-size: 11px;")
        self.val2 = QLabel("N/A")
        self.val2.setFont(QFont("Segoe UI", 16, QFont.Bold))
        stat2.addWidget(lbl2)
        stat2.addWidget(self.val2)
        
        stats_layout.addLayout(stat1)
        stats_layout.addLayout(stat2)
        lg_layout.addLayout(stats_layout)
        
        self.launch_btn = QPushButton("Launch Game")
        self.launch_btn.setCursor(Qt.PointingHandCursor)
        self.launch_btn.setStyleSheet("QPushButton { background-color: #e11324; color: white; padding: 10px; font-weight: bold; border-radius: 2px; margin-top: 10px; } QPushButton:hover { background-color: #ff1c2f; }")
        lg_layout.addWidget(self.launch_btn)
        
        left_grid.addWidget(last_game_box, 0, 0, 1, 1)
        
        recent_box = QFrame()
        recent_box.setStyleSheet("background-color: #181818; border: 1px solid #282828; border-radius: 4px;")
        rc_layout = QVBoxLayout(recent_box)
        rc_layout.setContentsMargins(20, 15, 20, 15)
        
        rc_title = QLabel("RECENT GAMES")
        rc_title.setFont(QFont("Segoe UI", 8, QFont.Bold))
        rc_title.setStyleSheet("color: #8e8e8e; letter-spacing: 0.5px;")
        rc_layout.addWidget(rc_title)
        
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setStyleSheet("QScrollArea { border: none; background: transparent; } QScrollBar { width: 0px; height: 0px; }")
        
        scroll_widget = QWidget()
        self.recent_grid = QHBoxLayout(scroll_widget)
        self.recent_grid.setContentsMargins(0, 10, 0, 0)
        self.recent_grid.setSpacing(15)
        
        scroll.setWidget(scroll_widget)
        rc_layout.addWidget(scroll)
        left_grid.addWidget(recent_box, 0, 1, 1, 1)
        
        banner_box = QFrame()
        banner_box.setStyleSheet("QFrame { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #1a1a1a, stop:1 #0a0a0a); border: 1px solid #282828; border-radius: 4px; }")
        banner_layout = QVBoxLayout(banner_box)
        banner_layout.setContentsMargins(35, 35, 35, 35)
        banner_layout.setAlignment(Qt.AlignmentFlag.AlignCenter)
        
        b_text1 = QLabel("AMD Software")
        b_text1.setFont(QFont("Segoe UI", 24, QFont.Bold))
        b_text1.setStyleSheet("color: white;")
        
        b_text2 = QLabel("Adrenalin Edition")
        b_text2.setFont(QFont("Segoe UI", 14))
        b_text2.setStyleSheet("color: #8e8e8e; margin-top: 4px;")
        
        b_link = QPushButton("Click for AMD Software Information")
        b_link.setCursor(Qt.PointingHandCursor)
        b_link.setStyleSheet("QPushButton { color: #e11324; font-size: 13px; font-weight: bold; text-align: left; margin-top: 20px; background: transparent; } QPushButton:hover { color: #ff1c2f; text-decoration: underline; }")
        b_link.clicked.connect(lambda: QDesktopServices.openUrl(QUrl("https://amd.com")))
        
        banner_layout.addWidget(b_text1)
        banner_layout.addWidget(b_text2)
        banner_layout.addWidget(b_link)
        
        left_grid.addWidget(banner_box, 1, 0, 1, 2)
        left_grid.setRowStretch(0, 1)
        left_grid.setRowStretch(1, 1)
        left_grid.setColumnStretch(0, 1)
        left_grid.setColumnStretch(1, 2)
        
        self.body_layout.addLayout(left_grid, stretch=3)
        
        right_panel = QVBoxLayout()
        right_panel.setSpacing(15)
        
        driver_box = QFrame()
        driver_box.setStyleSheet("background-color: #181818; border: 1px solid #282828; border-radius: 4px;")
        drv_layout = QVBoxLayout(driver_box)
        drv_layout.setContentsMargins(20, 15, 20, 15)
        
        drv_header = QLabel("DRIVER & SOFTWARE ↗")
        drv_header.setFont(QFont("Segoe UI", 8, QFont.Bold))
        drv_header.setStyleSheet("color: #8e8e8e; letter-spacing: 0.5px;")
        drv_layout.addWidget(drv_header)
        
        drv_ver = QLabel("Up To Date")
        drv_ver.setFont(QFont("Segoe UI", 13, QFont.Bold))
        drv_ver.setStyleSheet("color: #ffffff; margin-top: 8px;")
        drv_layout.addWidget(drv_ver)
        
        drv_btn = QPushButton("Check for Updates")
        drv_btn.setCursor(Qt.PointingHandCursor)
        drv_btn.setStyleSheet("QPushButton { background-color: #242424; color: white; padding: 8px; font-size: 12px; border-radius: 2px; margin-top: 10px; font-weight: bold; } QPushButton:hover { background-color: #303030; }")
        drv_btn.clicked.connect(lambda: self.switch_tab_file("Driver"))
        drv_layout.addWidget(drv_btn)
        
        right_panel.addWidget(driver_box)
        
        link_box = QFrame()
        link_box.setStyleSheet("background-color: #181818; border: 1px solid #282828; border-radius: 4px;")
        lnk_layout = QVBoxLayout(link_box)
        lnk_layout.setContentsMargins(20, 15, 20, 15)
        
        lnk_header = QLabel("AMD LINK STATUS ↗")
        lnk_header.setFont(QFont("Segoe UI", 8, QFont.Bold))
        lnk_header.setStyleSheet("color: #8e8e8e; letter-spacing: 0.5px;")
        lnk_layout.addWidget(lnk_header)
        
        lnk_status = QLabel("Not Connected")
        lnk_status.setFont(QFont("Segoe UI", 12, QFont.Bold))
        lnk_status.setStyleSheet("color: #8e8e8e; margin-top: 5px;")
        lnk_layout.addWidget(lnk_status)
        
        right_panel.addWidget(link_box)
        
        learn_box = QFrame()
        learn_box.setStyleSheet("background-color: #181818; border: 1px solid #282828; border-radius: 4px;")
        lrn_layout = QVBoxLayout(learn_box)
        lrn_layout.setContentsMargins(20, 15, 20, 15)
        
        lrn_header = QLabel("LEARN")
        lrn_header.setFont(QFont("Segoe UI", 8, QFont.Bold))
        lrn_header.setStyleSheet("color: #8e8e8e; letter-spacing: 0.5px;")
        lrn_layout.addWidget(lrn_header)
        
        lrn_status = QLabel("BOOST YOUR GAME")
        lrn_status.setFont(QFont("Segoe UI", 12, QFont.Bold))
        lrn_status.setStyleSheet("color: white; margin-top: 5px;")
        lrn_layout.addWidget(lrn_status)
        
        right_panel.addWidget(learn_box)
        right_panel.addStretch()
        
        self.body_layout.addLayout(right_panel, stretch=1)
        self.scan_steam_games()

    def render_gaming_view(self):
        left_panel = QVBoxLayout()
        left_panel.setSpacing(15)
        
        sec_title = QLabel("GAMING INFRASTRUCTURE & VULKAN OPTIMIZATION")
        sec_title.setFont(QFont("Segoe UI", 10, QFont.Bold))
        sec_title.setStyleSheet("color: #8e8e8e; letter-spacing: 0.5px;")
        left_panel.addWidget(sec_title)
        
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setStyleSheet("QScrollArea { border: none; background: transparent; } QScrollBar { width: 0px; height: 0px; }")
        
        scroll_widget = QWidget()
        scroll_widget.setStyleSheet("background-color: transparent;")
        games_layout = QVBoxLayout(scroll_widget)
        games_layout.setContentsMargins(0, 0, 0, 0)
        games_layout.setSpacing(12)
        
        has_gamemode = False
        has_mangohud = False
        for p in ["/usr/bin/gamemoderun", "/usr/local/bin/gamemoderun"]:
            if os.path.exists(p): has_gamemode = True
        for p in ["/usr/bin/mangohud", "/usr/local/bin/mangohud"]:
            if os.path.exists(p): has_mangohud = True
            
        steam_paths = [
            os.path.expanduser("~/.steam/steam/steamapps/*.acf"),
            os.path.expanduser("~/.local/share/Steam/steamapps/*.acf"),
            os.path.expanduser("~/.var/app/com.valvesoftware.Steam/.local/share/Steam/steamapps/*.acf")
        ]
        
        acf_files = []
        for path in steam_paths:
            files = glob.glob(path)
            if files: acf_files.extend(files)
            
        blacklisted_appids = ["228980", "250820", "1070560", "1391110", "1628350", "1826330", "1493710", "2172720", "291410", "223300"]
        detected_games = []
        
        for acf in acf_files:
            try:
                with open(acf, 'r', errors='ignore') as f:
                    name_val = ""
                    appid_val = ""
                    for line in f:
                        if '"name"' in line:
                            p = line.split('"')
                            if len(p) >= 4: name_val = p[3]
                        if '"appid"' in line:
                            p = line.split('"')
                            if len(p) >= 4: appid_val = p[3]
                    if name_val and appid_val and appid_val not in blacklisted_appids:
                        low_name = name_val.lower()
                        if not any(x in low_name for x in ["proton", "runtime", "steamworks", "soldat", "linux rolling", "scout", "heavy medicine"]):
                            detected_games.append((name_val, appid_val))
            except Exception:
                pass
                
        if not detected_games:
            no_games = QLabel("No Valid Steam Games Detected (Runtimes & Tooling Excluded)")
            no_games.setFont(QFont("Segoe UI", 12, QFont.Bold))
            no_games.setStyleSheet("color: #d32f2f;")
            games_layout.addWidget(no_games)
        else:
            for g_name, g_id in detected_games:
                g_frame = QFrame()
                g_frame.setStyleSheet("background-color: #181818; border: 1px solid #282828; border-radius: 4px;")
                g_inner = QVBoxLayout(g_frame)
                g_inner.setContentsMargins(20, 15, 20, 15)
                g_inner.setSpacing(10)
                
                top_line = QHBoxLayout()
                title_lbl = QLabel(g_name)
                title_lbl.setFont(QFont("Segoe UI", 13, QFont.Bold))
                title_lbl.setStyleSheet("color: white; border: none;")
                
                id_lbl = QLabel(f"AppID: {g_id}")
                id_lbl.setFont(QFont("Segoe UI", 10))
                id_lbl.setStyleSheet("color: #666666; border: none;")
                
                top_line.addWidget(title_lbl)
                top_line.addStretch()
                top_line.addWidget(id_lbl)
                g_inner.addLayout(top_line)
                
                launch_opts = []
                if has_gamemode: launch_opts.append("gamemoderun")
                if has_mangohud: launch_opts.append("mangohud")
                
                valve_games = ["counter-strike", "dota", "half-life", "portal", "left 4 dead", "team fortress"]
                is_valve = any(v in g_name.lower() for v in valve_games)
                
                if is_valve:
                    launch_opts.append("%command% -nojoy -novid")
                else:
                    launch_opts.insert(0, "RADV_PERFTEST=ngg AMD_DEBUG=nongg VK_ICD_FILENAMES=/usr/share/vulkan/icd.d/radeon_icd.x86_64.json")
                    launch_opts.append("%command%")
                    
                cmd_string = " ".join(launch_opts)
                cmd_entry = QLineEdit(cmd_string)
                cmd_entry.setReadOnly(True)
                cmd_entry.setFont(QFont("Monospace", 9))
                cmd_entry.setStyleSheet("QLineEdit { background-color: #0c0c0c; color: #00ff00; border: 1px solid #222222; padding: 6px; border-radius: 2px; }")
                
                copy_btn = QPushButton("Copy Options")
                copy_btn.setCursor(Qt.PointingHandCursor)
                copy_btn.setFixedWidth(110)
                copy_btn.setStyleSheet("QPushButton { background-color: #242424; color: white; padding: 6px; font-size: 11px; font-weight: bold; border-radius: 2px; } QPushButton:hover { background-color: #303030; }")
                copy_btn.clicked.connect(lambda checked=False, text=cmd_string: QApplication.clipboard().setText(text))
                
                opt_layout = QHBoxLayout()
                opt_layout.addWidget(cmd_entry)
                opt_layout.addWidget(copy_btn)
                g_inner.addLayout(opt_layout)
                games_layout.addWidget(g_frame)
                
        games_layout.addStretch()
        scroll.setWidget(scroll_widget)
        left_panel.addWidget(scroll)
        self.body_layout.addLayout(left_panel, stretch=3)
        
        right_panel = QVBoxLayout()
        right_panel.setSpacing(15)
        
        status_box = QFrame()
        status_box.setStyleSheet("background-color: #181818; border: 1px solid #282828; border-radius: 4px;")
        status_layout = QVBoxLayout(status_box)
        status_layout.setContentsMargins(20, 15, 20, 15)
        
        status_header = QLabel("GRAPHICS PIPELINE API")
        status_header.setFont(QFont("Segoe UI", 8, QFont.Bold))
        status_header.setStyleSheet("color: #8e8e8e; letter-spacing: 0.5px;")
        status_layout.addWidget(status_header)
        
        vulkan_ver = "Not Available"
        opengl_ver = "Not Available"
        try:
            vk_out = subprocess.check_output(["vulkaninfo", "--summary"], stderr=subprocess.DEVNULL).decode()
            for line in vk_out.splitlines():
                if "Vulkan API Version" in line or "api_version" in line:
                    vulkan_ver = line.split(":")[-1].strip()
        except Exception:
            try:
                gl_out = subprocess.check_output(["glxinfo"], stderr=subprocess.DEVNULL).decode()
                for line in gl_out.splitlines():
                    if "Vulkan" in line: vulkan_ver = line.strip()
            except Exception: pass
            
        try:
            gl_out = subprocess.check_output(["glxinfo"], stderr=subprocess.DEVNULL).decode()
            for line in gl_out.splitlines():
                if "OpenGL version string" in line:
                    opengl_ver = line.split(":")[-1].strip()
        except Exception: pass
        
        grid = QGridLayout()
        grid.setContentsMargins(0, 10, 0, 0)
        grid.setSpacing(10)
        
        lbl_vk = QLabel("Vulkan Core API:")
        lbl_vk.setStyleSheet("color: #8e8e8e;")
        val_vk = QLabel(vulkan_ver)
        val_vk.setStyleSheet("color: #00ff00; font-weight: bold;")
        
        lbl_gl = QLabel("OpenGL Profile:")
        lbl_gl.setStyleSheet("color: #8e8e8e;")
        val_gl = QLabel(opengl_ver if opengl_ver != "Not Available" else "Mesa DRI Profile")
        val_gl.setStyleSheet("color: white;")
        
        lbl_gm = QLabel("Feral GameMode:")
        lbl_gm.setStyleSheet("color: #8e8e8e;")
        val_gm = QLabel("ACTIVE" if has_gamemode else "NOT FOUND")
        val_gm.setStyleSheet("color: #00ff00; font-weight: bold;" if has_gamemode else "color: #ff1c2f; font-weight: bold;")
        
        lbl_mh = QLabel("MangoHud Layer:")
        lbl_mh.setStyleSheet("color: #8e8e8e;")
        val_mh = QLabel("ACTIVE" if has_mangohud else "NOT FOUND")
        val_mh.setStyleSheet("color: #00ff00; font-weight: bold;" if has_mangohud else "color: #ff1c2f; font-weight: bold;")
        
        grid.addWidget(lbl_vk, 0, 0)
        grid.addWidget(val_vk, 0, 1)
        grid.addWidget(lbl_gl, 1, 0)
        grid.addWidget(val_gl, 1, 1)
        grid.addWidget(lbl_gm, 2, 0)
        grid.addWidget(val_gm, 2, 1)
        grid.addWidget(lbl_mh, 3, 0)
        grid.addWidget(val_mh, 3, 1)
        status_layout.addLayout(grid)
        
        right_panel.addWidget(status_box)
        right_panel.addStretch()
        self.body_layout.addLayout(right_panel, stretch=1)

    def render_performance_view(self):
        left_panel = QVBoxLayout()
        left_panel.setSpacing(15)
        
        sec_title = QLabel("PERFORMANCE ENGINE & INFRASTRUCTURE MONITOR")
        sec_title.setFont(QFont("Segoe UI", 10, QFont.Bold))
        sec_title.setStyleSheet("color: #8e8e8e; letter-spacing: 0.5px;")
        left_panel.addWidget(sec_title)
        
        is_nvidia = os.path.exists("/proc/driver/nvidia/version")
        is_intel = False
        try:
            if os.path.exists("/sys/class/drm/"):
                for card in os.listdir("/sys/class/drm/"):
                    if card.startswith("card") and not "-" in card:
                        vendor_file = f"/sys/class/drm/{card}/device/vendor"
                        if os.path.exists(vendor_file):
                            with open(vendor_file, "r") as f:
                                if "0x8086" in f.read():
                                    is_intel = True
        except Exception:
            pass
            
        if is_nvidia or is_intel:
            err_frame = QFrame()
            err_frame.setStyleSheet("background-color: #181818; border: 1px solid #ff1c2f; border-radius: 4px;")
            err_layout = QVBoxLayout(err_frame)
            err_layout.setContentsMargins(30, 30, 30, 30)
            
            err_title = QLabel("HARDWARE INCOMPATIBILITY ERROR")
            err_title.setFont(QFont("Segoe UI", 14, QFont.Bold))
            err_title.setStyleSheet("color: #ff1c2f; border: none;")
            
            vendor_name = "NVIDIA" if is_nvidia else "INTEL"
            err_desc = QLabel(f"Active hardware sub-system detected as {vendor_name}. AMD Software telemetry pipeline requires native AMD Radeon hardware. Monitoring is disabled for non-AMD graphic platforms.")
            err_desc.setFont(QFont("Segoe UI", 11))
            err_desc.setWordWrap(True)
            err_desc.setStyleSheet("color: white; margin-top: 10px; border: none;")
            
            err_layout.addWidget(err_title)
            err_layout.addWidget(err_desc)
            left_panel.addWidget(err_frame)
            left_panel.addStretch()
            self.body_layout.addLayout(left_panel, stretch=3)
            self.body_layout.addStretch(1)
            return

        has_lact = False
        for p in ["/usr/bin/lact", "/usr/local/bin/lact", "/bin/lact"]:
            if os.path.exists(p): has_lact = True
        if not has_lact:
            try:
                subprocess.check_output(["which", "lact"], stderr=subprocess.DEVNULL)
                has_lact = True
            except Exception: pass

        if not has_lact:
            lact_frame = QFrame()
            lact_frame.setStyleSheet("background-color: #181818; border: 1px solid #ff9800; border-radius: 4px;")
            lact_layout = QVBoxLayout(lact_frame)
            lact_layout.setContentsMargins(25, 25, 25, 25)
            
            l_title = QLabel("LACT CONTROLLER UTILITY MISSING")
            l_title.setFont(QFont("Segoe UI", 13, QFont.Bold))
            l_title.setStyleSheet("color: #ff9800; border: none;")
            
            l_desc = QLabel("Low-level AMD metric tracing requires the Linux AMDGPU Controller daemon framework. Please download and install LACT package manually from the official software support repository page to initialize core clock charts.")
            l_desc.setFont(QFont("Segoe UI", 11))
            l_desc.setWordWrap(True)
            l_desc.setStyleSheet("color: white; margin-top: 10px; border: none;")
            
            lact_layout.addWidget(l_title)
            lact_layout.addWidget(l_desc)
            left_panel.addWidget(lact_frame)
        else:
            dash_frame = QFrame()
            dash_frame.setStyleSheet("background-color: #181818; border: 1px solid #282828; border-radius: 4px;")
            dash_layout = QVBoxLayout(dash_frame)
            dash_layout.setContentsMargins(25, 25, 25, 25)
            
            d_title = QLabel("LACT TELEMETRY INTERFACE IS ACTIVE")
            d_title.setFont(QFont("Segoe UI", 13, QFont.Bold))
            d_title.setStyleSheet("color: #00ff00; border: none;")
            
            open_btn = QPushButton("Launch LACT GUI Controller")
            open_btn.setCursor(Qt.PointingHandCursor)
            open_btn.setStyleSheet("QPushButton { background-color: #e11324; color: white; padding: 10px 20px; font-weight: bold; border-radius: 2px; margin-top: 15px; } QPushButton:hover { background-color: #ff1c2f; }")
            open_btn.clicked.connect(lambda: subprocess.Popen(["lact-gui"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL))
            
            dash_layout.addWidget(d_title)
            dash_layout.addWidget(open_btn)
            left_panel.addWidget(dash_frame)
            
        left_panel.addStretch()
        self.body_layout.addLayout(left_panel, stretch=3)
        
        right_panel = QVBoxLayout()
        right_panel.setSpacing(15)
        
        sys_box = QFrame()
        sys_box.setStyleSheet("background-color: #181818; border: 1px solid #282828; border-radius: 4px;")
        sys_layout = QVBoxLayout(sys_box)
        sys_layout.setContentsMargins(20, 15, 20, 15)
        
        sys_header = QLabel("KERNEL & GRAPHICS LAYER INFO")
        sys_header.setFont(QFont("Segoe UI", 8, QFont.Bold))
        sys_header.setStyleSheet("color: #8e8e8e; letter-spacing: 0.5px;")
        sys_layout.addWidget(sys_header)
        
        kernel_ver = "Unknown Kernel"
        mesa_ver = "Unknown Mesa"
        chip_ver = "AMD Radeon Chip"
        try:
            kernel_ver = subprocess.check_output(["uname", "-r"]).decode().strip()
        except Exception: pass
        try:
            glx_out = subprocess.check_output(["glxinfo"], stderr=subprocess.DEVNULL).decode()
            for line in glx_out.splitlines():
                if "OpenGL version string" in line: mesa_ver = line.split(":")[-1].strip()
        except Exception: pass
        try:
            if os.path.exists("/sys/class/drm/"):
                for card in os.listdir("/sys/class/drm/"):
                    if card.startswith("card") and not "-" in card:
                        with open(f"/sys/class/drm/{card}/device/device", "r") as f:
                            chip_ver = f"AMDgpu Device ID: {f.read().strip()}"
                            break
        except Exception: pass
        
        drv_ver_str = self.get_system_driver()
        
        grid = QGridLayout()
        grid.setContentsMargins(0, 10, 0, 0)
        grid.setSpacing(10)
        
        lbl_k = QLabel("Linux Kernel:")
        lbl_k.setStyleSheet("color: #8e8e8e;")
        val_k = QLabel(kernel_ver)
        val_k.setStyleSheet("color: white;")
        
        lbl_d = QLabel("Driver Core:")
        lbl_d.setStyleSheet("color: #8e8e8e;")
        val_d = QLabel(drv_ver_str)
        val_d.setStyleSheet("color: #e11324; font-weight: bold;")
        
        lbl_m = QLabel("Mesa Framework:")
        lbl_m.setStyleSheet("color: #8e8e8e;")
        val_m = QLabel(mesa_ver)
        val_m.setStyleSheet("color: white;")
        
        lbl_c = QLabel("GPU ASIC Chip:")
        lbl_c.setStyleSheet("color: #8e8e8e;")
        val_c = QLabel(chip_ver)
        val_c.setStyleSheet("color: white;")
        
        grid.addWidget(lbl_k, 0, 0)
        grid.addWidget(val_k, 0, 1)
        grid.addWidget(lbl_d, 1, 0)
        grid.addWidget(val_d, 1, 1)
        grid.addWidget(lbl_m, 2, 0)
        grid.addWidget(val_m, 2, 1)
        grid.addWidget(lbl_c, 3, 0)
        grid.addWidget(val_c, 3, 1)
        sys_layout.addLayout(grid)
        
        right_panel.addWidget(sys_box)
        right_panel.addStretch()
        self.body_layout.addLayout(right_panel, stretch=1)

    def render_driver_view(self):
        left_panel = QVBoxLayout()
        left_panel.setSpacing(15)
        
        drv_ver_str = self.get_system_driver()
        
        header_panel = QHBoxLayout()
        sec_title = QLabel("DRIVER SYSTEM INFRASTRUCTURE")
        sec_title.setFont(QFont("Segoe UI", 10, QFont.Bold))
        sec_title.setStyleSheet("color: #8e8e8e; letter-spacing: 0.5px;")
        
        ver_lbl = QLabel(f"Active Graphics Driver: {drv_ver_str}")
        ver_lbl.setFont(QFont("Segoe UI", 10, QFont.Bold))
        ver_lbl.setStyleSheet("color: #e11324;")
        
        header_panel.addWidget(sec_title)
        header_panel.addStretch()
        header_panel.addWidget(ver_lbl)
        left_panel.addLayout(header_panel)
        
        term_frame = QFrame()
        term_frame.setStyleSheet("background-color: #050505; border: 1px solid #202020; border-radius: 4px;")
        term_layout = QVBoxLayout(term_frame)
        term_layout.setContentsMargins(15, 15, 15, 15)
        term_layout.setSpacing(10)
        
        top_control = QHBoxLayout()
        distro_lbl = QLabel("Select Target Sub-System Distribution:")
        distro_lbl.setFont(QFont("Segoe UI", 10))
        distro_lbl.setStyleSheet("color: #8e8e8e;")
        
        distro_combo = QComboBox()
        distro_combo.setFixedWidth(320)
        distro_combo.setStyleSheet("""
            QComboBox { background-color: #181818; color: white; border: 1px solid #303030; padding: 5px; border-radius: 2px; font-family: 'Segoe UI'; font-size: 12px; }
            QComboBox::drop-down { border: none; }
            QComboBox QAbstractItemView { background-color: #181818; color: white; selection-background-color: #e11324; }
        """)
        
        distros = {
            "Ubuntu / Debian / Mint / Pop!_OS (APT)": "pkexec apt update && pkexec apt full-upgrade",
            "Arch Linux / Manjaro / EndeavourOS (Pacman)": "pkexec pacman -Syu",
            "Fedora / RHEL Workstation (DNF)": "pkexec dnf upgrade --refresh",
            "Fedora Silverblue / Kinoite (RPM-Ostree)": "pkexec rpm-ostree upgrade",
            "Void Linux (XBPS Standard)": "pkexec xbps-install -Syu",
            "Void Linux (XBPS Direct)": "pkexec xbps-install -Su",
            "Solus OS (Eopkg)": "pkexec eopkg upgrade",
            "openSUSE Tumbleweed / Leap (Zypper)": "pkexec zypper dup",
            "Gentoo Linux (Portage Core)": "pkexec emerge --sync && pkexec emerge -auDN @world",
            "Mageia Linux (URPMI Engine)": "pkexec urpmi --auto-select",
            "Alpine Linux (APK Infrastructure)": "pkexec apk upgrade --available",
            "Slackware Linux (Slackpkg Native)": "pkexec slackpkg update && pkexec slackpkg upgrade-all",
            "Slackware Variant (Slapt-get)": "pkexec slapt-get --upgrade",
            "NixOS (Nix Channel Environment)": "pkexec nix-channel --update && pkexec nixos-rebuild switch",
            "GNU Guix System (Guix Transactional)": "pkexec guix pull && pkexec guix system reconfigure /etc/config.scm",
            "Mariner OS / Photon (TDNF Engine)": "pkexec tdnf update",
            "Clear Linux OS (Swupd Layer)": "pkexec swupd update",
            "KISS Linux (KISS Package Utilities)": "pkexec kiss u",
            "Pisi Linux (Pisi Package Management)": "pkexec pisi upgrade",
            "Source Mage (Sorcery Spells)": "pkexec sorcery update",
            "NetBSD Env / Portable (Pkgin)": "pkexec pkgin upgrade",
            "OpenBSD Infrastructure (Pkg_add)": "pkexec pkg_add -u",
            "OpenWrt Embedded Layer (Opkg)": "pkexec opkg update && pkexec opkg upgrade",
            "Manjaro Core Mirror-Sync (Pacman-Mirrors)": "pkexec pacman-mirrors -g && pkexec pacman -Syu",
            "Fedora Major System Upgrade (DNF-Plugin)": "pkexec dnf system-upgrade download",
            "RedHat Minimal Core (Microdnf)": "pkexec microdnf upgrade",
            "CRUX Linux Native Ports (Netpkg)": "pkexec netpkg",
            "SliTaz GNU/Linux (Tazpkg Utility)": "pkexec tazpkg upgrade",
            "Universal Sandboxed Deployment (Flatpak Core)": "pkexec flatpak update",
            "Canonical Container Isolation (Snap Daemon)": "pkexec snap refresh"
        }
        
        for d_name in distros.keys():
            distro_combo.addItem(d_name)
            
        run_btn = QPushButton("Execute Task")
        run_btn.setCursor(Qt.PointingHandCursor)
        run_btn.setStyleSheet("QPushButton { background-color: #e11324; color: white; font-weight: bold; font-size: 11px; padding: 6px 15px; border-radius: 2px; } QPushButton:hover { background-color: #ff1c2f; }")
        
        top_control.addWidget(distro_lbl)
        top_control.addWidget(distro_combo)
        top_control.addStretch()
        top_control.addWidget(run_btn)
        term_layout.addLayout(top_control)
        
        term_output = QTextEdit()
        term_output.setReadOnly(True)
        term_output.setFont(QFont("Monospace", 10))
        term_output.setStyleSheet("QTextEdit { background-color: #020202; color: #00ff00; border: 1px solid #151515; border-radius: 2px; padding: 5px; }")
        term_layout.addWidget(term_output)
        
        prompt_widget = QWidget()
        prompt_layout = QHBoxLayout(prompt_widget)
        prompt_layout.setContentsMargins(0, 0, 0, 0)
        prompt_layout.setSpacing(5)
        
        prompt_lbl = QLabel("amd_smi_emulator@kernel:~#")
        prompt_lbl.setFont(QFont("Monospace", 10, QFont.Bold))
        prompt_lbl.setStyleSheet("color: #e11324;")
        
        term_input = QLineEdit()
        term_input.setFont(QFont("Monospace", 10))
        term_input.setStyleSheet("QLineEdit { background-color: transparent; color: white; border: none; }")
        
        prompt_layout.addWidget(prompt_lbl)
        prompt_layout.addWidget(term_input)
        term_layout.addWidget(prompt_widget)
        
        left_panel.addWidget(term_frame)
        self.body_layout.addLayout(left_panel, stretch=3)
        
        right_panel = QVBoxLayout()
        right_panel.setSpacing(15)
        



        side_box = QFrame()
        side_box.setStyleSheet("background-color: #181818; border: 1px solid #282828; border-radius: 4px;")
        side_layout = QVBoxLayout(side_box)
        side_layout.setContentsMargins(20, 15, 20, 15)
        




        side_header = QLabel("EMULATOR LOGIC")
        side_header.setFont(QFont("Segoe UI", 8, QFont.Bold))
        side_header.setStyleSheet("color: #8e8e8e; letter-spacing: 0.5px;")
        side_layout.addWidget(side_header)
        



        side_desc = QLabel("The virtual sandbox captures raw stdout descriptors directly from kernel pipes.")
        side_desc.setFont(QFont("Segoe UI", 11))
        side_desc.setWordWrap(True)
        side_desc.setStyleSheet("color: white; margin-top: 5px;")
        side_layout.addWidget(side_desc)
        




        
        right_panel.addWidget(side_box)
        right_panel.addStretch()
        self.body_layout.addLayout(right_panel, stretch=1)
        
        self.process = QProcess(self)
        
        def append_stdout():
            data = self.process.readAllStandardOutput().data().decode(errors='ignore')
            term_output.insertPlainText(data)
            term_output.ensureCursorVisible()
            
        def append_stderr():
            data = self.process.readAllStandardError().data().decode(errors='ignore')
            term_output.insertPlainText(data)
            term_output.ensureCursorVisible()
            
        self.process.readyReadStandardOutput.connect(append_stdout)
        self.process.readyReadStandardError.connect(append_stderr)
        
        def run_sys_cmd(cmd_text):
            if not cmd_text.strip():
                return
            term_output.insertPlainText(f"\nexecuting: {cmd_text}\n")
            term_output.ensureCursorVisible()
            if self.process.state() == QProcess.ProcessState.Running:
                self.process.terminate()
                self.process.waitForFinished()
            self.process.start("bash", ["-c", cmd_text])
            term_input.clear()
            
        run_btn.clicked.connect(lambda checked=False: run_sys_cmd(distros[distro_combo.currentText()]))
        term_input.returnPressed.connect(lambda: run_sys_cmd(term_input.text()))
        term_output.setText("=== AMD Adrenalin Kernel Upgrade Sandbox Emulator initialized ===\nSelect terminal profile target distribution above or type custom pipelines below (e.g. fastfetch).\n")

    def render_placeholder_view(self):
        blank_lbl = QLabel(f"{self.active_tab.upper()} Tab Content - Module Placeholder\nCreate {self.active_tab.lower()}.py to inject real engine logic.")
        blank_lbl.setFont(QFont("Segoe UI", 12, QFont.Bold))
        blank_lbl.setStyleSheet("color: #8e8e8e;")
        blank_lbl.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.body_layout.addWidget(blank_lbl)

    def get_system_driver(self):
        try:
            cards = [d for d in os.listdir("/sys/class/drm/") if d.startswith("card") and not "-" in d]
            for card in cards:
                if os.path.exists(f"/sys/class/drm/{card}/device/driver/module"):
                    real_link = os.readlink(f"/sys/class/drm/{card}/device/driver/module")
                    module_name = os.path.basename(real_link)
                    ver_path = f"/sys/module/{module_name}/version"
                    if os.path.exists(ver_path):
                        with open(ver_path, "r") as f:
                            return f"AMD {module_name} kernel module v{f.read().strip()}"
                    out = subprocess.check_output(["modinfo", "-F", "version", module_name], stderr=subprocess.DEVNULL).decode().strip()
                    if out:
                        return f"AMD {module_name} ({out})"
                    return f"AMD {module_name} (In-Kernel Source)"
        except Exception:
            pass
        return "amdgpu (Direct Rendering Infrastructure Engine)"












    def switch_tab_file(self, target):
        if self.process and self.process.state() == QProcess.ProcessState.Running:
            self.process.terminate()
            self.process.waitForFinished()
            
        self.active_tab = target
        
        if target.lower() == "home":
            if self.root_widget:
                self.root_widget.deleteLater()
            self.build_main_frame()
            return
            
        mapping = {
            "gaming": "gaming",
            "performance": "performance"
        }















        file_key = mapping.get(target.lower(), target.lower())
        
        if file_key != "driver" and file_key != "gaming" and file_key != "performance":
            try:
                mod = __import__(file_key)
                if hasattr(mod, "init_tab_logic"):
                    if self.root_widget:
                        self.root_widget.deleteLater()
                    mod.init_tab_logic(self)
                    return
            except ImportError:
                pass
                
        if self.root_widget:
            self.root_widget.deleteLater()
        self.build_main_frame()

    def scan_steam_games(self):
        steam_paths = [
            os.path.expanduser("~/.steam/steam/steamapps/*.acf"),
            os.path.expanduser("~/.local/share/Steam/steamapps/*.acf"),
            os.path.expanduser("~/.var/app/com.valvesoftware.Steam/.local/share/Steam/steamapps/*.acf")
        ]
        
        acf_files = []
        is_flatpak_steam = False
        
        for path in steam_paths:
            files = glob.glob(path)
            if files:
                acf_files.extend(files)
                if "com.valvesoftware.Steam" in path:
                    is_flatpak_steam = True
                    
        if not acf_files:
            self.show_no_steam_msg("Steam is not installed")
            return
            
        if is_flatpak_steam:
            self.show_no_steam_msg("Flatpak Steam don't support")
            return
            
        games = []
        for acf in acf_files:
            try:
                with open(acf, 'r', errors='ignore') as f:
                    name_val = ""
                    appid_val = ""
                    for line in f:
                        if '"name"' in line:
                            parts = line.split('"')
                            if len(parts) >= 4:
                                name_val = parts[3]
                        if '"appid"' in line:
                            parts = line.split('"')
                            if len(parts) >= 4:
                                appid_val = parts[3]
                    if name_val and appid_val and appid_val != "228980":
                        games.append((name_val, appid_val))
            except Exception:
                pass
                
        if not games:
            self.show_no_steam_msg("No Steam Games Found")
            return
            
        first_game_name, first_game_id = games[0]
        self.lg_name.setText(first_game_name)
        self.val1.setText("12")
        self.val2.setText("64.5")
        
        try:
            self.launch_btn.clicked.disconnect()
        except (TypeError, RuntimeError):
            pass
        self.launch_btn.clicked.connect(lambda checked=False, id=first_game_id: os.system(f"steam steam://rungameid/{id} &"))
        
        for name, appid in games[:4]:
            g_box = QFrame()
            g_box.setFixedSize(110, 120)
            g_box.setStyleSheet("background-color: #202020; border: 1px solid #282828; border-radius: 4px;")
            g_layout = QVBoxLayout(g_box)
            g_layout.setContentsMargins(10, 10, 10, 10)




            g_name = QLabel(name)
            g_name.setFont(QFont("Segoe UI", 9, QFont.Bold))
            g_name.setWordWrap(True)
            g_name.setAlignment(Qt.AlignmentFlag.AlignCenter)
            g_name.setStyleSheet("color: white;")






            g_btn = QPushButton("Play")
            g_btn.setCursor(Qt.PointingHandCursor)
            g_btn.setStyleSheet("QPushButton { background-color: #282828; color: white; font-size: 10px; font-weight: bold; padding: 4px; border-radius: 2px; } QPushButton:hover { background-color: #383838; }")
            g_btn.clicked.connect(lambda checked=False, id=appid: os.system(f"steam steam://rungameid/{id} &"))
            





            g_layout.addWidget(g_name)
            g_layout.addStretch()
            g_layout.addWidget(g_btn)
            self.recent_grid.addWidget(g_box)
            
        self.recent_grid.addStretch()





    def show_no_steam_msg(self, text):
        self.lg_name.setText("System Notification")
        self.val1.setText("-")
        self.val2.setText("-")
        self.launch_btn.setEnabled(False)
        self.launch_btn.setStyleSheet("background-color: #282828; color: #555555;")




        msg = QLabel(text)
        msg.setFont(QFont("Segoe UI", 12, QFont.Bold))
        msg.setStyleSheet("color: #d32f2f;")
        self.recent_grid.addWidget(msg)
        self.recent_grid.addStretch()





if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = AdrenalinApp()
    window.show()
    sys.exit(app.exec())
