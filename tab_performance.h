#pragma once
#include <QMouseEvent>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QPainterPath>
#include <vector>
#include <cmath>
#include "amd_smi/amdsmi.h"

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
            int y = height() * i / 4;
            p.drawLine(0, y, width(), y);
            int x = width() * i / 4;
            p.drawLine(x, 0, x, height());
        }

        p.setPen(QPen(QColor("#606060"), 1));
        p.drawText(5, height() - 5, "30°C / 0%");
        p.drawText(width() - 55, 15, "100°C / 100%");

        QPainterPath path;
        for(size_t i = 0; i < points.size(); ++i) {
            QPoint pt = toScreen(points[i]);
            if(i == 0) path.moveTo(pt);
            else path.lineTo(pt);
        }
        p.setPen(QPen(QColor("#FF002B"), 2));
        p.drawPath(path);

        for(size_t i = 0; i < points.size(); ++i) {
            QPoint pt = toScreen(points[i]);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(i == (size_t)activePointIdx ? "#FFFFFF" : "#FF002B"));
            p.drawEllipse(pt, 5, 5);
        }
    }

    void mousePressEvent(QMouseEvent *event) override {
        for(size_t i = 0; i < points.size(); ++i) {
            QPoint pt = toScreen(points[i]);
            if(std::hypot(event->pos().x() - pt.x(), event->pos().y() - pt.y()) < 8) {
                activePointIdx = i;
                update();
                break;
            }
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if(activePointIdx != -1 && event->buttons() & Qt::LeftButton) {
            Point pt = toLogical(event->pos());

            int minT = (activePointIdx == 0) ? 20 : points[activePointIdx-1].temp + 1;
            int maxT = (activePointIdx == points.size()-1) ? 100 : points[activePointIdx+1].temp - 1;

            pt.temp = std::clamp(pt.temp, minT, maxT);
            pt.speed = std::clamp(pt.speed, 0, 100);

            points[activePointIdx] = pt;
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
        int x = ((lp.temp - 20) * width()) / 80;
        int y = height() - ((lp.speed * height()) / 100);
        return QPoint(x, y);
    }

    Point toLogical(const QPoint &sp) {
        int temp = 20 + (sp.x() * 80) / width();
        int speed = ((height() - sp.y()) * 100) / height();
        return Point{temp, speed};
    }
};

class PerformanceTab : public QWidget {
    Q_OBJECT
public:
    explicit PerformanceTab(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
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

        QLabel *tuningTitle = new QLabel("SMART FAN CURVE OPTIMIZATION", this);
        tuningTitle->setStyleSheet("font-size: 11px; font-weight: bold; color: #808080;");
        tuningLayout->addWidget(tuningTitle);

        curveEditor = new FanCurveEditor(this);
        tuningLayout->addWidget(curveEditor);

        QHBoxLayout *ctrls = new QHBoxLayout();
        QPushButton *applyBtn = new QPushButton("Apply Smart Curve", this);
        applyBtn->setFixedHeight(32);
        applyBtn->setStyleSheet("QPushButton { background-color: #FF002B; color: #FFFFFF; font-weight: bold; border-radius: 3px; font-size: 12px; padding: 0 20px; }"
                                 "QPushButton:hover { background-color: #E00024; }");
        connect(applyBtn, &QPushButton::clicked, this, &PerformanceTab::writeFanCurveToHardware);

        ctrls->addStretch();
        ctrls->addWidget(applyBtn);
        tuningLayout->addLayout(ctrls);

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
    uint32_t deviceIndex = 0;
    amdsmi_processor_handle processorHandle;

    QWidget* createMetricCard(QString title, QString rawText) {
        QWidget *card = new QWidget(this);
        card->setStyleSheet("background-color: #1A1A1A; border: 1px solid #2D2D2D; border-radius: 4px;");
        card->setFixedHeight(120);

        QVBoxLayout *l = new QVBoxLayout(card);
        l->setContentsMargins(15, 12, 15, 12);

        QLabel *t = new QLabel(title, card);
        t->setStyleSheet("font-size: 10px; font-weight: bold; color: #808080;");

        QLabel *body = new QLabel(rawText, card);
        body->setObjectName("body");
        body->setStyleSheet("font-size: 12px; color: #E0E0E0; font-family: monospace; line-height: 1.4;");

        l->addWidget(t);
        l->addWidget(body);
        l->addStretch();
        return card;
    }

    void initHardwareAccess() {
        if (amdsmi_init(AMDSMI_INIT_AMD_GPUS) == AMDSMI_STATUS_SUCCESS) {
            uint32_t socket_count = 0;
            amdsmi_get_socket_handles(&socket_count, nullptr);
            if (socket_count > 0) {
                std::vector<amdsmi_socket_handle> sockets(socket_count);
                amdsmi_get_socket_handles(&socket_count, sockets.data());
                uint32_t device_count = 0;
                amdsmi_get_processor_handles(sockets[0], &device_count, nullptr);
                if (device_count > 0) {
                    std::vector<amdsmi_processor_handle> processors(device_count);
                    amdsmi_get_processor_handles(sockets[0], &device_count, processors.data());
                    processorHandle = processors[0];
                    hasAmdSmi = true;
                }
            }
        }
    }

    void pollMetrics() {
        pollGpuVram();
        pollCpu();
        pollRam();
    }

    void pollGpuVram() {
        if (!hasAmdSmi) {
            gpuCard->findChild<QLabel*>("body")->setText("GPU Temp: N/A (NVIDIA Host Mode)\nClock: Core Idle\nPower: Management Disabled");
            vramCard->findChild<QLabel*>("body")->setText("VRAM Used: N/A\nTotal: N/A\nClock: Idle");
            return;
        }

        uint64_t temp = 0, clock = 0, power = 0;
        amdsmi_vram_usage_t vram;

        amdsmi_get_gpu_cache_temperature(processorHandle, AMDSMI_TEMPERATURE_EDGE, &temp);

        amdsmi_engine_usage_t engine;
        amdsmi_get_gpu_activity_descriptor(processorHandle, &engine);

        amdsmi_get_gpu_vram_usage(processorHandle, &vram);

        gpuCard->findChild<QLabel*>("body")->setText(QString("GPU Temp: %1°C\nClock: %2 MHz\nPower: %3 W").arg(temp).arg(engine.gfx_activity).arg(power));
        vramCard->findChild<QLabel*>("body")->setText(QString("VRAM Used: %1 MB\nTotal: %2 MB\nClock: VRAM Dynamic").arg(vram.vram_value).arg(vram.vram_total));
    }

    void pollCpu() {
        QFile statFile("/proc/stat");
        if (!statFile.open(QIODevice::ReadOnly | QIODevice::Text)) return;
        QTextStream in(&statFile);
        QString line = in.readLine();
        statFile.close();

        QStringList tokens = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if(tokens.size() < 5) return;

        static long long lastUser = 0, lastNice = 0, lastSys = 0, lastIdle = 0;
        long long user = tokens[1].toLongLong();
        long long nice = tokens[2].toLongLong();
        long long sys = tokens[3].toLongLong();
        long long idle = tokens[4].toLongLong();

        long long totalDiff = (user + nice + sys + idle) - (lastUser + lastNice + lastSys + lastIdle);
        long long idleDiff = idle - lastIdle;
        int load = 0;

        if (totalDiff > 0) {
            load = 100 * (totalDiff - idleDiff) / totalDiff;
        }

        lastUser = user;
        lastNice = nice;
        lastSys = sys;
        lastIdle = idle;

        QFile cpuInfo("/proc/cpuinfo");
        double mhz = 0.0;
        if (cpuInfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream infoIn(&cpuInfo);
            while (!infoIn.atEnd()) {
                QString l = infoIn.readLine();
                if (l.startsWith("cpu MHz")) {
                    mhz = l.split(":").last().trimmed().toDouble();
                    break;
                }
            }
            cpuInfo.close();
        }

        QLabel *cpuBody = cpuCard->findChild<QLabel*>("body");
        if (cpuBody) {
            cpuBody->setText(QString("CPU Load: %1 %\nTemp: System Scanned\nFreq: %2 MHz").arg(load).arg((int)mhz));
        }
    }

    void pollRam() {
        QFile memFile("/proc/meminfo");
        if (!memFile.open(QIODevice::ReadOnly | QIODevice::Text)) return;
        QTextStream in(&memFile);
        long long total = 0, free = 0, available = 0, swapTotal = 0, swapFree = 0;

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(QRegularExpression("\\s+"));
            if (parts.size() < 2) continue;

            if (line.startsWith("MemTotal:")) total = parts[1].toLongLong();
            if (line.startsWith("MemFree:")) free = parts[1].toLongLong();
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

    void writeFanCurveToHardware() {
        if (!hasAmdSmi) return;

        QString hwmonPath = "";
        QDir dir("/sys/class/drm/card0/device/hwmon");
        if (dir.exists()) {
            QStringList subdirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            if (!subdirs.isEmpty()) {
                hwmonPath = "/sys/class/drm/card0/device/hwmon/" + subdirs.first() + "/";
            }
        }

        if (hwmonPath.isEmpty()) return;

        QFile manualMode(hwmonPath + "pwm1_enable");
        if (manualMode.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&manualMode);
            out << "1";
            manualMode.close();
        }

        QFile curveFile(hwmonPath + "fan_curve");
        if (curveFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&curveFile);
            for (const auto& pt : curveEditor->points) {
                int pwmValue = (pt.speed * 255) / 100;
                out << pt.temp << " " << pwmValue << "\n";
            }
            curveFile.close();
        }
    }
};
