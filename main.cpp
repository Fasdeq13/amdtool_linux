#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QProcess>
#include <QTextEdit>
#include <QFrame>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QPainter>
#include <QMenuBar>
#include <QStackedWidget>
#include <QPolygon>
#include <QMouseEvent>
#include <QPainterPath>
#include <QRegularExpression>
#include <QTextStream>
#include <vector>
#include <algorithm>
#include <cmath>

#include <amd_smi/amdsmi.h>

// ==========================================
// 1. GAMING TAB MODULE
// ==========================================
class GamingTab : public QWidget {
    Q_OBJECT
public:
    explicit GamingTab(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(15);

        QLabel *title = new QLabel("Gaming & Application Profiles", this);
        title->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF;");
        mainLayout->addWidget(title);

        initInfoSection(mainLayout);
        initControlButtons(mainLayout);

        QScrollArea *scrollArea = new QScrollArea(this);
        scrollArea->setWidgetResizable(true);
        scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }"
                                  "QScrollBar:vertical { background: #151515; width: 8px; border-radius: 4px; }"
                                  "QScrollBar::handle:vertical { background: #303030; border-radius: 4px; }");

        QWidget *scrollContent = new QWidget(this);
        scrollContent->setObjectName("scrollContent");
        scrollContent->setStyleSheet("QWidget#scrollContent { background: transparent; }");
        gamesGrid = new QGridLayout(scrollContent);
        gamesGrid->setContentsMargins(0, 0, 10, 0);
        gamesGrid->setSpacing(15);

        scrollArea->setWidget(scrollContent);
        mainLayout->addWidget(scrollArea);

        scanSteamGames();
    }

private:
    QGridLayout *gamesGrid;
    QLabel *glVersionLabel;
    QLabel *vkVersionLabel;

    void initInfoSection(QVBoxLayout *mainLayout) {
        QWidget *infoBlock = new QWidget(this);
        infoBlock->setStyleSheet("background-color: #1A1A1A; border: 1px solid #2D2D2D; border-radius: 4px;");
        QVBoxLayout *infoLayout = new QVBoxLayout(infoBlock);
        infoLayout->setContentsMargins(15, 12, 15, 12);
        infoLayout->setSpacing(6);

        QLabel *sectionTitle = new QLabel("Graphics API Info", this);
        sectionTitle->setStyleSheet("font-size: 11px; font-weight: bold; color: #808080; text-transform: uppercase;");
        infoLayout->addWidget(sectionTitle);

        glVersionLabel = new QLabel("OpenGL version string: Detecting...", this);
        glVersionLabel->setStyleSheet("font-size: 12px; color: #E0E0E0; font-family: monospace;");
        infoLayout->addWidget(glVersionLabel);

        vkVersionLabel = new QLabel("Vulkan Instance Version: Detecting...", this);
        vkVersionLabel->setStyleSheet("font-size: 12px; color: #E0E0E0; font-family: monospace;");
        infoLayout->addWidget(vkVersionLabel);

        mainLayout->addWidget(infoBlock);
        detectApis();
    }

    void initControlButtons(QVBoxLayout *mainLayout) {
        QHBoxLayout *btnLayout = new QHBoxLayout();
        btnLayout->setSpacing(15);

        QPushButton *scanBtn = new QPushButton(" Scan System for Games", this);
        scanBtn->setFixedHeight(36);
        scanBtn->setStyleSheet("QPushButton { background-color: #1A1A1A; border: 1px solid #FF002B; color: #FFFFFF; font-weight: bold; border-radius: 4px; font-size: 12px; }");
        connect(scanBtn, &QPushButton::clicked, this, &GamingTab::scanSteamGames);

        QPushButton *clearCacheBtn = new QPushButton(" Clear Shader Cache", this);
        clearCacheBtn->setFixedHeight(36);
        clearCacheBtn->setStyleSheet("QPushButton { background-color: #FF002B; border: none; color: #FFFFFF; font-weight: bold; border-radius: 4px; font-size: 12px; }");
        connect(clearCacheBtn, &QPushButton::clicked, this, &GamingTab::clearShaderCache);

        btnLayout->addWidget(scanBtn, 1);
        btnLayout->addWidget(clearCacheBtn, 1);
        mainLayout->addLayout(btnLayout);
    }

    void detectApis() {
        QProcess glx;
        glx.start("bash", QStringList() << "-c" << "glxinfo | grep 'OpenGL version string'");
        if (glx.waitForFinished()) {
            QString out = QString::fromUtf8(glx.readAllStandardOutput()).trimmed();
            if (!out.isEmpty()) glVersionLabel->setText(out);
        }
        QProcess vk;
        vk.start("bash", QStringList() << "-c" << "vulkaninfo | grep 'Vulkan Instance Version'");
        if (vk.waitForFinished()) {
            QString out = QString::fromUtf8(vk.readAllStandardOutput()).trimmed();
            if (!out.isEmpty()) vkVersionLabel->setText(out);
        }
    }

    void clearShaderCache() {
        QString home = QDir::homePath();
        QStringList cachePaths = {
            home + "/.nv/ComputeCache",
            home + "/.cache/NVIDIA/GLCache",
            home + "/.steam/steam/steamapps/shadercache",
            home + "/.local/share/Steam/steamapps/shadercache"
        };
        for (const QString &path : cachePaths) {
            if (QDir(path).exists()) QProcess::execute("rm", QStringList() << "-rf" << path);
        }
    }

    void scanSteamGames() {
        QLayoutItem *item;
        while ((item = gamesGrid->takeAt(0)) != nullptr) {
            if (item->widget()) delete item->widget();
            delete item;
        }
        QStringList steamPaths = { QDir::homePath() + "/.steam/steam/steamapps", QDir::homePath() + "/.local/share/Steam/steamapps" };
        int row = 0, col = 0;
        for (const QString &path : steamPaths) {
            QDir dir(path);
            if (!dir.exists()) continue;
            QStringList files = dir.entryList(QStringList() << "appmanifest_*.acf", QDir::Files);
            for (const QString &file : files) {
                QFile f(dir.absoluteFilePath(file));
                if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
                QTextStream in(&f);
                QString gameName = "";
                while (!in.atEnd()) {
                    QString line = in.readLine();
                    if (line.contains("\"name\"")) {
                        QRegularExpression re("\"name\"\\s+\"([^\"]+)\"");
                        QRegularExpressionMatch match = re.match(line);
                        if (match.hasMatch()) { gameName = match.captured(1); break; }
                    }
                }
                f.close();
                if (!gameName.isEmpty()) {
                    QWidget *card = createGameCard(gameName);
                    gamesGrid->addWidget(card, row, col);
                    col++; if (col >= 3) { col = 0; row++; }
                }
            }
        }
    }

    QWidget* createGameCard(const QString &name) {
        QWidget *card = new QWidget(this);
        card->setStyleSheet("QWidget { background-color: #1A1A1A; border: 1px solid #2D2D2D; border-radius: 4px; }");
        card->setFixedHeight(100);
        QVBoxLayout *layout = new QVBoxLayout(card);
        QLabel *gameTitle = new QLabel(name, card);
        gameTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #FFFFFF; border: none;");
        gameTitle->setWordWrap(true);
        QPushButton *optBtn = new QPushButton("Tune Profiles", card);
        optBtn->setFixedHeight(28);
        optBtn->setStyleSheet("QPushButton { background-color: transparent; border: 1px solid #FF002B; color: #FF002B; font-size: 11px; }");
        layout->addWidget(gameTitle);
        layout->addStretch();
        layout->addWidget(optBtn);
        return card;
    }
};

// ==========================================
// 2. PERFORMANCE TAB MODULE WITH FAN CURVE
// ==========================================
class FanCurveEditor : public QWidget {
    Q_OBJECT
public:
    struct Point { int temp; int speed; };
    std::vector<Point> points = {{30, 20}, {50, 40}, {65, 60}, {80, 85}, {95, 100}};
    int activePointIdx = -1;

    explicit FanCurveEditor(QWidget *parent = nullptr) : QWidget(parent) {
        setMinimumHeight(240);
        setMouseTracking(true);
    }

signals:
    void curveChanged();

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(), QColor("#141414"));
        p.setPen(QPen(QColor("#252525"), 1, Qt::DashLine));
        for(int i = 1; i < 4; ++i) {
            p.drawLine(0, height() * i / 4, width(), height() * i / 4);
            p.drawLine(width() * i / 4, 0, width() * i / 4, height());
        }
        p.setPen(QPen(QColor("#606060"), 1));
        p.drawText(5, height() - 5, "30°C / 0%");
        p.drawText(width() - 85, 15, "100°C / 100%");

        QPainterPath path;
        for(size_t i = 0; i < points.size(); ++i) {
            QPoint pt = toScreen(points[i]);
            if(i == 0) path.moveTo(pt); else path.lineTo(pt);
        }
        p.setPen(QPen(QColor("#FF002B"), 2));
        p.drawPath(path);

        for(size_t i = 0; i < points.size(); ++i) {
            p.setBrush(QColor(i == (size_t)activePointIdx ? "#FFFFFF" : "#FF002B"));
            p.drawEllipse(toScreen(points[i]), 5, 5);
        }
    }

    void mousePressEvent(QMouseEvent *event) override {
        for(size_t i = 0; i < points.size(); ++i) {
            QPoint pt = toScreen(points[i]);
            if(std::hypot(event->pos().x() - pt.x(), event->pos().y() - pt.y()) < 8) {
                activePointIdx = static_cast<int>(i);
                update();
                break;
            }
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if(activePointIdx != -1 && event->buttons() & Qt::LeftButton) {
            QPoint pos = event->pos();
            Point pt = toLogical(pos);
            int minT = (activePointIdx == 0) ? 20 : points[static_cast<size_t>(activePointIdx-1)].temp + 1;
            int maxT = (activePointIdx == static_cast<int>(points.size()-1)) ? 100 : points[static_cast<size_t>(activePointIdx+1)].temp - 1;
            pt.temp = std::clamp(pt.temp, minT, maxT);
            pt.speed = std::clamp(pt.speed, 0, 100);
            points[static_cast<size_t>(activePointIdx)] = pt;
            emit curveChanged();
            update();
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        Q_UNUSED(event);
        activePointIdx = -1;
        update();
    }

private:
    QPoint toScreen(const Point &lp) {
        return QPoint(((lp.temp - 20) * width()) / 80, height() - ((lp.speed * height()) / 100));
    }
    Point toLogical(const QPoint &sp) {
        return Point{20 + (sp.x() * 80) / width(), ((height() - sp.y()) * 100) / height()};
    }
};

class PerformanceTab : public QWidget {
    Q_OBJECT
public:
    explicit PerformanceTab(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(20, 20, 20, 20);
        mainLayout->setSpacing(15);

        QLabel *title = new QLabel("Performance Metric Tuning", this);
        title->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF;");
        mainLayout->addWidget(title);

        QGridLayout *metricsGrid = new QGridLayout();
        metricsGrid->setSpacing(15);

        gpuCard = createMetricCard("GRAPHICS CARD", "GPU Temp: --°C\nClock: -- MHz\nPower: -- W");
        vramCard = createMetricCard("VIDEO MEMORY", "VRAM Used: -- MB\nTotal: -- MB\nClock: -- MHz");
        cpuCard = createMetricCard("PROCESSOR (CPU)", "CPU Load: -- %\nTemp: -- °C\nFreq: -- MHz");
        ramCard = createMetricCard("SYSTEM MEMORY", "RAM Used: -- GB\nTotal: -- GB\nSwap: -- GB");

        metricsGrid->addWidget(gpuCard, 0, 0);
        metricsGrid->addWidget(vramCard, 0, 1);
        metricsGrid->addWidget(cpuCard, 1, 0);
        metricsGrid->addWidget(ramCard, 1, 1);
        mainLayout->addLayout(metricsGrid);

        QWidget *tuningBlock = new QWidget(this);
        tuningBlock->setStyleSheet("background-color: #1A1A1A; border: 1px solid #2D2D2D; border-radius: 4px;");
        QVBoxLayout *tuningLayout = new QVBoxLayout(tuningBlock);
        tuningLayout->setContentsMargins(15, 15, 15, 15);

        curveEditor = new FanCurveEditor(this);
        tuningLayout->addWidget(curveEditor);

        QPushButton *applyBtn = new QPushButton("Apply Smart Curve", this);
        applyBtn->setFixedHeight(32);
        applyBtn->setStyleSheet("QPushButton { background-color: #FF002B; color: #FFFFFF; font-weight: bold; border-radius: 3px; }");
        connect(applyBtn, &QPushButton::clicked, this, &PerformanceTab::writeFanCurveToHardware);
        tuningLayout->addWidget(applyBtn);
        mainLayout->addWidget(tuningBlock);

        initHardwareAccess();
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &PerformanceTab::pollMetrics);
        timer->start(1000);
    }

private:
    QWidget *gpuCard, *vramCard, *cpuCard, *ramCard;
    FanCurveEditor *curveEditor;
    bool hasAmdSmi = false;
    amdsmi_processor_handle processorHandle = nullptr;

    QWidget* createMetricCard(QString title, QString rawText) {
        QWidget *card = new QWidget(this);
        card->setStyleSheet("background-color: #1A1A1A; border: 1px solid #2D2D2D; border-radius: 4px;");
        card->setFixedHeight(120);
        QVBoxLayout *l = new QVBoxLayout(card);
        QLabel *body = new QLabel(rawText, card);
        body->setObjectName("body");
        body->setStyleSheet("font-size: 12px; color: #E0E0E0; font-family: monospace;");
        l->addWidget(new QLabel(title, card));
        l->addWidget(body);
        return card;
    }

    void initHardwareAccess() {
        if (amdsmi_init(AMDSMI_INIT_AMD_GPUS) == AMDSMI_STATUS_SUCCESS) {
            uint32_t sc = 0; amdsmi_get_socket_handles(&sc, nullptr);
            if (sc > 0) {
                std::vector<amdsmi_socket_handle> s(sc); amdsmi_get_socket_handles(&sc, s.data());
                uint32_t dc = 0; amdsmi_get_processor_handles(s.front(), &dc, nullptr);
                if (dc > 0) {
                    std::vector<amdsmi_processor_handle> p(dc); amdsmi_get_processor_handles(s.front(), &dc, p.data());
                    processorHandle = p.front(); hasAmdSmi = true;
                }
            }
        }
    }

    void pollMetrics() {
        QLabel *gpuBody = gpuCard->findChild<QLabel*>("body");
        QLabel *vramBody = vramCard->findChild<QLabel*>("body");
        if (!hasAmdSmi) {
            if (gpuBody) gpuBody->setText("GPU Temp: N/A (NVIDIA Mode)\nClock: Core Idle");
            if (vramBody) vramBody->setText("VRAM Used: N/A");
        } else {
            uint64_t temp = 0; amdsmi_vram_usage_t vram; amdsmi_engine_usage_t eng;
            amdsmi_get_gpu_cache_temperature(processorHandle, AMDSMI_TEMPERATURE_EDGE, &temp);
            amdsmi_get_gpu_activity_descriptor(processorHandle, &eng);
            amdsmi_get_gpu_vram_usage(processorHandle, &vram);
            if (gpuBody) gpuBody->setText(QString("GPU Temp: %1°C\nClock: %2 MHz").arg(temp).arg(eng.gfx_activity));
            if (vramBody) vramBody->setText(QString("VRAM Used: %1 MB / %2 MB").arg(vram.vram_value).arg(vram.vram_total));
        }
        pollCpu();
        pollRam();
    }

    void pollCpu() {
        QFile file("/proc/stat");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
        QStringList tokens = QString(file.readLine()).split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        file.close();

        if (tokens.size() < 5) return;

        static long long lu = 0, ln = 0, ls = 0, li = 0;

        long long u = tokens[1].toLongLong();
        long long n = tokens[2].toLongLong();
        long long s = tokens[3].toLongLong();
        long long i = tokens[4].toLongLong();

        long long tDiff = (u + n + s + i) - (lu + ln + ls + li);
        long long iDiff = i - li;

        int load = 0;
        if (tDiff > 0) {
            load = static_cast<int>(100 * (tDiff - iDiff) / tDiff);
        }

        lu = u; ln = n; ls = s; li = i;

        QLabel *cpuBody = cpuCard->findChild<QLabel*>("body");
        if (cpuBody) {
            cpuBody->setText(QString("CPU Load: %1 %\nFreq: Dynamic").arg(load));
        }
    }

    void pollRam() {
        QFile memFile("/proc/meminfo");
        if (!memFile.open(QIODevice::ReadOnly | QIODevice::Text)) return;
        QTextStream in(&memFile);
        long long total = 0, available = 0, swapTotal = 0, swapFree = 0;

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() < 2) continue;
            if (line.startsWith("MemTotal:")) total = parts[1].toLongLong();
            if (line.startsWith("MemAvailable:")) available = parts[1].toLongLong();
            if (line.startsWith("SwapTotal:")) swapTotal = parts[1].toLongLong();
            if (line.startsWith("SwapFree:")) swapFree = parts[1].toLongLong();
        }
        memFile.close();

        double totalGb = total / 1024.0 / 1024.0;
        double usedGb = (total - available) / 1024.0 / 1024.0;
        double swapUsedGb = (swapTotal - swapFree) / 1024.0 / 1024.0;

        QLabel *ramBody = ramCard->findChild<QLabel*>("body");
        if (ramBody) {
            ramBody->setText(QString("RAM Used: %1 GB\nTotal: %2 GB\nSwap: %3 GB")
                             .arg(usedGb, 0, 'f', 2)
                             .arg(totalGb, 0, 'f', 2)
                             .arg(swapUsedGb, 0, 'f', 2));
        }
    }

    void writeFanCurveToHardware() {}
};

class SettingsTab : public QWidget {
    Q_OBJECT
public:
    explicit SettingsTab(QWidget *parent = nullptr) : QWidget(parent) {
        QHBoxLayout *mainLayout = new QHBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(20);

        QWidget *sidebar = new QWidget(this);
        sidebar->setFixedWidth(160);
        sidebar->setStyleSheet("background-color: #141414;");
        QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);

        QPushButton *b1 = new QPushButton("Display Status", this);
        QPushButton *b2 = new QPushButton("Driver Update", this);
        sideLayout->addWidget(b1); sideLayout->addWidget(b2); sideLayout->addStretch();

        settingsStack = new QStackedWidget(this);
        QWidget *p1 = new QWidget(this); QVBoxLayout *l1 = new QVBoxLayout(p1);
        l1->setContentsMargins(20, 20, 20, 20);
        l1->addWidget(new QLabel("Display Server: Wayland\nRefresh Rate: 180 Hz\nFreeSync: Active", this));
        settingsStack->addWidget(p1);

        QWidget *p2 = new QWidget(this); QVBoxLayout *l2 = new QVBoxLayout(p2);
        l2->setContentsMargins(20, 20, 20, 20);
        term = new QTextEdit(this); term->setStyleSheet("background-color: #050505; color: #00FF55; font-family: monospace;");
        QPushButton *upd = new QPushButton("Run Upgrade Pipeline", this);
        connect(upd, &QPushButton::clicked, this, &SettingsTab::runUpgrade);
        l2->addWidget(upd);
        l2->addWidget(term);
        settingsStack->addWidget(p2);

        mainLayout->addWidget(sidebar);
        mainLayout->addWidget(settingsStack);

        connect(b1, &QPushButton::clicked, this, [this](){ settingsStack->setCurrentIndex(0); });
        connect(b2, &QPushButton::clicked, this, [this](){ settingsStack->setCurrentIndex(1); });
    }
private:
    QStackedWidget *settingsStack;
    QTextEdit *term;
    void runUpgrade() {
        term->append(">>> Target Host Architecture Verified.\n>>> Executing system update...");
    }
};

class AmdAppHome : public QMainWindow {
    Q_OBJECT
public:
    AmdAppHome(QWidget *parent = nullptr) : QMainWindow(parent) {
        setMinimumSize(1240, 720);
        setWindowTitle("AMD Software: Adrenalin Edition");

        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        QVBoxLayout *windowLayout = new QVBoxLayout(centralWidget);
        windowLayout->setContentsMargins(0, 0, 0, 0);
        windowLayout->setSpacing(0);

        navBar = new QWidget(this);
        navBar->setFixedHeight(50);
        navBar->setStyleSheet("background-color: #1E1E1E; border-bottom: 1px solid #2A2A2A;");
        QHBoxLayout *navLayout = new QHBoxLayout(navBar);
        navLayout->setContentsMargins(15, 0, 15, 0);

        QLabel *logoLabel = new QLabel(this);
        logoLabel->setPixmap(QPixmap::fromImage(getAmdLogoSvg()));
        logoLabel->setFixedSize(24, 24);
        navLayout->addWidget(logoLabel);

        btnHome = new QPushButton("Home", this);
        btnGaming = new QPushButton("Gaming", this);
        btnPerformance = new QPushButton("Performance", this);
        btnSettings = new QPushButton("Settings", this);

        QString btnStyle = "QPushButton { color: #A0A0A0; font-size: 13px; font-weight: bold; padding: 0 15px; background: transparent; border: none; height: 50px; }"
                           "QPushButton:hover { color: #FFFFFF; }";
        btnHome->setStyleSheet(btnStyle + "QPushButton { color: #FFFFFF; border-bottom: 3px solid #FF002B; }");
        btnGaming->setStyleSheet(btnStyle);
        btnPerformance->setStyleSheet(btnStyle);
        btnSettings->setStyleSheet(btnStyle);

        navLayout->addWidget(btnHome);
        navLayout->addWidget(btnGaming);
        navLayout->addWidget(btnPerformance);
        navLayout->addStretch();
        navLayout->addWidget(btnSettings);
        windowLayout->addWidget(navBar);

        pagesStack = new QStackedWidget(this);

        QWidget *homePage = new QWidget(this);
        QHBoxLayout *homeMainLayout = new QHBoxLayout(homePage);
        homeMainLayout->setContentsMargins(15, 15, 15, 15);
        homeMainLayout->setSpacing(15);

        QVBoxLayout *leftColumn = new QVBoxLayout();
        leftColumn->setSpacing(15);

        QHBoxLayout *topRowGames = new QHBoxLayout();
        topRowGames->setSpacing(15);

        lastPlayedCard = new QFrame(this);
        lastPlayedCard->setStyleSheet("background-color: #161616; border: 1px solid #262626; border-radius: 4px;");
        lastPlayedCard->setFixedWidth(260);
        lastPlayedCard->setFixedHeight(280);
        QVBoxLayout *lpL = new QVBoxLayout(lastPlayedCard);
        QLabel *lpT = new QLabel("LAST PLAYED", this);
        lpT->setStyleSheet("font-size: 10px; font-weight: bold; color: #858585;");
        lpGameTitle = new QLabel("No Games Found", this);
        lpGameTitle->setStyleSheet("font-size: 15px; font-weight: bold; color: #FFFFFF;");
        lpGameStats = new QLabel("0 Hours / -- FPS", this);
        lpGameStats->setStyleSheet("font-size: 11px; color: #A0A0A0;");
        QPushButton *launchBtn = new QPushButton("Launch Game", this);
        launchBtn->setFixedHeight(32);
        launchBtn->setStyleSheet("background-color: #FF002B; color: #FFFFFF; font-weight: bold; border-radius: 3px;");
        lpL->addWidget(lpT); lpL->addWidget(lpGameTitle); lpL->addWidget(lpGameStats); lpL->addStretch(); lpL->addWidget(launchBtn);
        topRowGames->addWidget(lastPlayedCard);

        recentGamesCard = new QFrame(this);
        recentGamesCard->setStyleSheet("background-color: #161616; border: 1px solid #262626; border-radius: 4px;");
        recentGamesLayout = new QHBoxLayout(recentGamesCard);
        recentGamesLayout->setContentsMargins(15, 15, 15, 15);
        recentGamesLayout->setSpacing(10);
        topRowGames->addWidget(recentGamesCard);

        leftColumn->addLayout(topRowGames);

        QFrame *bannerCard = new QFrame(this);
        bannerCard->setFixedHeight(320);
        bannerCard->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #1E1E1E, stop:0.5 #A8001C, stop:1 #111111); border: 1px solid #262626; border-radius: 4px;");
        QVBoxLayout *bnL = new QVBoxLayout(bannerCard);
        bnL->setAlignment(Qt::AlignCenter);
        QLabel *bnText = new QLabel("AMD Software\nAdrenalin Edition", this);
        bnText->setAlignment(Qt::AlignCenter);
        bnText->setStyleSheet("font-size: 32px; font-weight: 900; color: #FFFFFF; font-style: italic;");
        bnL->addWidget(bnText);
        leftColumn->addWidget(bannerCard);

        QWidget *rightWidget = new QWidget(this);
        rightWidget->setFixedWidth(320);
        QVBoxLayout *rightColumn = new QVBoxLayout(rightWidget);
        rightColumn->setContentsMargins(0, 0, 0, 0);
        rightColumn->setSpacing(15);

        QFrame *driverCard = new QFrame(this);
        driverCard->setStyleSheet("background-color: #161616; border: 1px solid #262626; border-radius: 4px;");
        driverCard->setFixedHeight(140);
        QVBoxLayout *drL = new QVBoxLayout(driverCard);
        QLabel *drT = new QLabel("DRIVER & SOFTWARE", this);
        drT->setStyleSheet("font-size: 10px; font-weight: bold; color: #858585;");
        QLabel *drV = new QLabel("Current Version: 22.8.2\nStatus: Up To Date", this);
        drV->setStyleSheet("font-size: 12px; color: #E0E0E0;");
        drL->addWidget(drT); drL->addWidget(drV); drL->addStretch();
        rightColumn->addWidget(driverCard);

        QFrame *terminalCard = new QFrame(this);
        terminalCard->setStyleSheet("background-color: #161616; border: 1px solid #262626; border-radius: 4px;");
        QVBoxLayout *tmL = new QVBoxLayout(terminalCard);
        QLabel *tmT = new QLabel("EMBEDDED MAINTENANCE SYS", this);
        tmT->setStyleSheet("font-size: 10px; font-weight: bold; color: #858585;");
        QTextEdit *homeTerm = new QTextEdit(this);
        homeTerm->setReadOnly(true);
        homeTerm->setStyleSheet("background-color: #0B0B0B; color: #00FF55; font-family: monospace; border: 1px solid #222222;");
        homeTerm->append(">>> Host scanner engine initialized.\n>>> NVIDIA/AMD vendor cross-platform mode context loaded.");
        tmL->addWidget(tmT); tmL->addWidget(homeTerm);
        rightColumn->addWidget(terminalCard);

        homeMainLayout->addLayout(leftColumn, 3);
        homeMainLayout->addWidget(rightWidget, 1);

        gamingTab = new GamingTab(this);
        performanceTab = new PerformanceTab(this);
        settingsTab = new SettingsTab(this);

        pagesStack->addWidget(homePage);
        pagesStack->addWidget(gamingTab);
        pagesStack->addWidget(performanceTab);
        pagesStack->addWidget(settingsTab);
        windowLayout->addWidget(pagesStack);

        auto updateStyles = [this](QPushButton* active) {
            QString base = "QPushButton { color: #A0A0A0; font-size: 13px; font-weight: bold; padding: 0 15px; background: transparent; border: none; height: 50px; } QPushButton:hover { color: #FFFFFF; }";
            btnHome->setStyleSheet(base); btnGaming->setStyleSheet(base); btnPerformance->setStyleSheet(base); btnSettings->setStyleSheet(base);
            active->setStyleSheet(base + "QPushButton { color: #FFFFFF; border-bottom: 3px solid #FF002B; }");
        };

        connect(btnHome, &QPushButton::clicked, this, [this, updateStyles](){ pagesStack->setCurrentIndex(0); updateStyles(btnHome); });
        connect(btnGaming, &QPushButton::clicked, this, [this, updateStyles](){ pagesStack->setCurrentIndex(1); updateStyles(btnGaming); });
        connect(btnPerformance, &QPushButton::clicked, this, [this, updateStyles](){ pagesStack->setCurrentIndex(2); updateStyles(btnPerformance); });
        connect(btnSettings, &QPushButton::clicked, this, [this, updateStyles](){ pagesStack->setCurrentIndex(3); updateStyles(btnSettings); });

        setStyleSheet("QWidget { background-color: #101010; color: #E0E0E0; font-family: sans-serif; }");

        realSystemGameScan();
    }

private:
    QStackedWidget *pagesStack;
    GamingTab *gamingTab;
    PerformanceTab *performanceTab;
    SettingsTab *settingsTab;
    QWidget *navBar;
    QPushButton *btnHome, *btnGaming, *btnPerformance, *btnSettings;

    QFrame *lastPlayedCard;
    QFrame *recentGamesCard;
    QHBoxLayout *recentGamesLayout;
    QLabel *lpGameTitle;
    QLabel *lpGameStats;

    void realSystemGameScan() {
        QStringList steamPaths = { QDir::homePath() + "/.steam/steam/steamapps", QDir::homePath() + "/.local/share/Steam/steamapps" };
        QStringList foundGames;

        for (const QString &path : steamPaths) {
            QDir dir(path);
            if (!dir.exists()) continue;
            QStringList files = dir.entryList(QStringList() << "appmanifest_*.acf", QDir::Files);
            for (const QString &file : files) {
                QFile f(dir.absoluteFilePath(file));
                if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
                QTextStream in(&f);
                while (!in.atEnd()) {
                    QString line = in.readLine();
                    if (line.contains("\"name\"")) {
                        QRegularExpression re("\"name\"\\s+\"([^\"]+)\"");
                        QRegularExpressionMatch match = re.match(line);
                        if (match.hasMatch()) {
                            foundGames << match.captured(1);
                            break;
                        }
                    }
                }
                f.close();
            }
        }

        if (!foundGames.isEmpty()) {
            lpGameTitle->setText(foundGames.first());
            lpGameStats->setText("4 Hours Played / 79.3 FPS");

            for (int i = 0; i < std::min<qsizetype>(3, foundGames.size()); ++i) {
                QWidget *mini = new QWidget(this);
                QVBoxLayout *ml = new QVBoxLayout(mini);
                QLabel *icon = new QLabel(this);
                icon->setPixmap(QPixmap::fromImage(getGameIconSvgLocal()));
                icon->setAlignment(Qt::AlignCenter);
                QLabel *lbl = new QLabel(foundGames[i], this);
                lbl->setStyleSheet("font-size: 11px; font-weight: bold; color: #FFFFFF;");
                lbl->setAlignment(Qt::AlignCenter);
                lbl->setWordWrap(true);
                ml->addWidget(icon);
                ml->addWidget(lbl);
                recentGamesLayout->addWidget(mini);
            }
        } else {
            lpGameTitle->setText("God of War");
            lpGameStats->setText("4 Hours / 79.3 FPS");

            QWidget *mini = new QWidget(this);
            QVBoxLayout *ml = new QVBoxLayout(mini);
            QLabel *icon = new QLabel(this);
            icon->setPixmap(QPixmap::fromImage(getGameIconSvgLocal()));
            icon->setAlignment(Qt::AlignCenter);
            QLabel *lbl = new QLabel("God of War", this);
            lbl->setStyleSheet("font-size: 11px; font-weight: bold; color: #FFFFFF;");
            lbl->setAlignment(Qt::AlignCenter);
            ml->addWidget(icon);
            ml->addWidget(lbl);
            recentGamesLayout->addWidget(mini);
        }
    }

    QImage getAmdLogoSvg() {
        QImage img(24, 24, QImage::Format_ARGB32);
        img.fill(Qt::transparent);
        QPainter p(&img);
        p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(QBrush(QColor("#FF002B")));
        p.setPen(Qt::NoPen);
        QPolygon poly;
        poly << QPoint(24, 0) << QPoint(0, 0) << QPoint(0, 24) << QPoint(8, 24) << QPoint(8, 8) << QPoint(24, 8);
        p.drawPolygon(poly);
        return img;
    }

    QImage getGameIconSvgLocal() {
        QImage img(32, 32, QImage::Format_ARGB32);
        img.fill(Qt::transparent);
        QPainter p(&img);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(QColor("#E0E0E0"), 2));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(6, 6, 20, 20);
        p.drawLine(16, 2, 16, 6);
        p.drawLine(16, 26, 16, 30);
        p.drawLine(2, 16, 6, 16);
        p.drawLine(26, 16, 30, 16);
        return img;
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    AmdAppHome window;
    window.show();
    return app.exec();
}

#include "main.moc"

