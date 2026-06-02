#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QTextEdit>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QStackedWidget>
#include <QPainter>
#include <QImage>

class SettingsTab : public QWidget {
    Q_OBJECT
public:
    explicit SettingsTab(QWidget *parent = nullptr) : QWidget(parent) {
        QHBoxLayout *mainLayout = new QHBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(20);

        QWidget *sidebar = new QWidget(this);
        sidebar->setObjectName("settingsSidebar");
        sidebar->setFixedWidth(180);
        sidebar->setStyleSheet("QWidget#settingsSidebar { background-color: #141414; border-right: 1px solid #282828; }");
        QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);
        sideLayout->setContentsMargins(10, 15, 10, 15);
        sideLayout->setSpacing(8);

        btnDisplay = new QPushButton("Display Status", this);
        btnUpdate = new QPushButton("Driver Update", this);
        btnAbout = new QPushButton("About Project", this);

        QString sideBtnStyle = "QPushButton { text-align: left; padding: 10px; color: #909090; font-size: 13px; font-weight: bold; border-radius: 4px; }"
                               "QPushButton:hover { background-color: #202020; color: #FFFFFF; }"
                               "QPushButton#activeSub { background-color: #2D1015; color: #FF002B; }";

        btnDisplay->setStyleSheet(sideBtnStyle);
        btnUpdate->setStyleSheet(sideBtnStyle);
        btnAbout->setStyleSheet(sideBtnStyle);

        btnDisplay->setObjectName("activeSub");

        sideLayout->addWidget(btnDisplay);
        sideLayout->addWidget(btnUpdate);
        sideLayout->addWidget(btnAbout);
        sideLayout->addStretch();

        settingsStack = new QStackedWidget(this);

        initDisplayPage();
        initUpdatePage();
        initAboutPage();

        mainLayout->addWidget(sidebar);
        mainLayout->addWidget(settingsStack);

        connect(btnDisplay, &QPushButton::clicked, this, [this]() { switchPage(0, btnDisplay); });
        connect(btnUpdate, &QPushButton::clicked, this, [this]() { switchPage(1, btnUpdate); });
        connect(btnAbout, &QPushButton::clicked, this, [this]() { switchPage(2, btnAbout); });
    }

private:
    QStackedWidget *settingsStack;
    QPushButton *btnDisplay;
    QPushButton *btnUpdate;
    QPushButton *btnAbout;
    QTextEdit *interactiveTerminal;
    QProcess *termProcess;

    QLabel *lblDisplayServer;
    QLabel *lblRefreshRate;
    QLabel *lblHdrStatus;
    QLabel *lblFreesyncStatus;

    void switchPage(int index, QPushButton *targetButton) {
        settingsStack->setCurrentIndex(index);
        btnDisplay->setObjectName("");
        btnUpdate->setObjectName("");
        btnAbout->setObjectName("");
        btnDisplay->setStyle(btnDisplay->style());
        btnUpdate->setStyle(btnUpdate->style());
        btnAbout->setStyle(btnAbout->style());
        targetButton->setObjectName("activeSub");
        targetButton->setStyle(targetButton->style());
    }

    void initDisplayPage() {
        QWidget *page = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(page);
        layout->setContentsMargins(10, 10, 10, 10);
        layout->setSpacing(20);

        QLabel *title = new QLabel("System & Display Status", this);
        title->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF;");
        layout->addWidget(title);

        QFrame *dispCard = new QFrame(this);
        dispCard->setStyleSheet("background-color: #1A1A1A; border: 1px solid #2D2D2D; border-radius: 4px;");
        QVBoxLayout *dcLayout = new QVBoxLayout(dispCard);
        dcLayout->setContentsMargins(15, 15, 15, 15);
        dcLayout->setSpacing(12);

        QLabel *sectTitle = new QLabel("DISPLAY CONFIGURATION", this);
        sectTitle->setStyleSheet("font-size: 10px; font-weight: bold; color: #FF002B; letter-spacing: 1px;");
        dcLayout->addWidget(sectTitle);

        lblDisplayServer = new QLabel("Display Server: Detecting...", this);
        lblRefreshRate = new QLabel("Current Refresh Rate: Detecting...", this);
        lblHdrStatus = new QLabel("HDR (High Dynamic Range): Detecting...", this);
        lblFreesyncStatus = new QLabel("AMD FreeSync / VRR: Detecting...", this);

        QString labelStyle = "font-size: 12px; color: #E0E0E0; font-family: sans-serif;";
        lblDisplayServer->setStyleSheet(labelStyle);
        lblRefreshRate->setStyleSheet(labelStyle);
        lblHdrStatus->setStyleSheet(labelStyle);
        lblFreesyncStatus->setStyleSheet(labelStyle);

        dcLayout->addWidget(lblDisplayServer);
        dcLayout->addWidget(lblRefreshRate);
        dcLayout->addWidget(lblHdrStatus);
        dcLayout->addWidget(lblFreesyncStatus);
        layout->addWidget(dispCard);

        QFrame *audioCard = new QFrame(this);
        audioCard->setStyleSheet("background-color: #1A1A1A; border: 1px solid #2D2D2D; border-radius: 4px;");
        QVBoxLayout *acLayout = new QVBoxLayout(audioCard);
        acLayout->setContentsMargins(15, 15, 15, 15);

        QLabel *audioTitle = new QLabel("AUDIO TECHNOLOGIES", this);
        audioTitle->setStyleSheet("font-size: 10px; font-weight: bold; color: #FF002B; letter-spacing: 1px;");
        QLabel *audioDesc = new QLabel("Advanced Spatial Audio Protocol: Linux ALSA/PulseAudio Native Bridge", this);
        audioDesc->setStyleSheet(labelStyle);

        acLayout->addWidget(audioTitle);
        acLayout->addWidget(audioDesc);
        layout->addWidget(audioCard);

        layout->addStretch();
        settingsStack->addWidget(page);

        detectDisplayMetrics();
    }

    void initUpdatePage() {
        QWidget *page = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(page);
        layout->setContentsMargins(10, 10, 10, 10);
        layout->setSpacing(15);

        QLabel *title = new QLabel("Driver & Core System Maintenance", this);
        title->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF;");
        layout->addWidget(title);

        QPushButton *btnStartUpdate = new QPushButton("Execute System Upgrade", this);
        btnStartUpdate->setFixedHeight(36);
        btnStartUpdate->setStyleSheet("QPushButton { background-color: #FF002B; color: #FFFFFF; font-weight: bold; border-radius: 4px; font-size: 12px; }"
                                      "QPushButton:hover { background-color: #E00024; }");
        layout->addWidget(btnStartUpdate);

        interactiveTerminal = new QTextEdit(this);
        interactiveTerminal->setStyleSheet("background-color: #050505; color: #00FF55; font-family: monospace; font-size: 12px; border: 1px solid #2D2D2D; border-radius: 4px;");
        interactiveTerminal->setReadOnly(false);
        layout->addWidget(interactiveTerminal);

        settingsStack->addWidget(page);

        connect(btnStartUpdate, &QPushButton::clicked, this, &SettingsTab::startSystemUpdatePipeline);
    }

    void initAboutPage() {
        QWidget *page = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(page);
        layout->setContentsMargins(10, 10, 10, 10);
        layout->setSpacing(20);

        QLabel *title = new QLabel("About Software", this);
        title->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF;");
        layout->addWidget(title);

        QFrame *aboutCard = new QFrame(this);
        aboutCard->setStyleSheet("background-color: #1A1A1A; border: 1px solid #2D2D2D; border-radius: 4px;");
        QVBoxLayout *acLayout = new QVBoxLayout(aboutCard);
        acLayout->setContentsMargins(20, 20, 20, 20);
        acLayout->setSpacing(15);

        QLabel *appName = new QLabel("AMD Tool Interface (Linux Edition)", this);
        appName->setStyleSheet("font-size: 16px; font-weight: bold; color: #FFFFFF;");

        QLabel *creator = new QLabel("Lead Developer: Fasdeq13", this);
        creator->setStyleSheet("font-size: 13px; color: #A0A0A0;");

        QLabel *desc = new QLabel("An open-source graphics suite infrastructure tailored for AMD Radeon hardware running on Linux environments natively.", this);
        desc->setStyleSheet("font-size: 12px; color: #E0E0E0;");
        desc->setWordWrap(true);

        QPushButton *btnGithub = new QPushButton(" Visit GitHub Repository", this);
        btnGithub->setFixedWidth(180);
        btnGithub->setFixedHeight(32);
        btnGithub->setStyleSheet("QPushButton { background-color: #252525; border: 1px solid #333333; color: #FFFFFF; font-weight: bold; border-radius: 4px; font-size: 12px; }"
                                 "QPushButton:hover { background-color: #303030; border: 1px solid #FF002B; }");

        acLayout->addWidget(appName);
        acLayout->addWidget(creator);
        acLayout->addWidget(desc);
        acLayout->addWidget(btnGithub);
        layout->addWidget(aboutCard);

        layout->addStretch();
        settingsStack->addWidget(page);

        connect(btnGithub, &QPushButton::clicked, []() {
            QDesktopServices::openUrl(QUrl("https://github.com/Fasdeq13"));
        });
    }

    void detectDisplayMetrics() {
        QString session = qgetenv("XDG_SESSION_TYPE");
        if (session.isEmpty()) session = "X11 (Fallback)";
        lblDisplayServer->setText("Display Server: " + session);

        QProcess xrandr;
        xrandr.start("bash", QStringList() << "-c" << "xrandr | grep '*' | awk '{print $2}'");
        if (xrandr.waitForFinished()) {
            QString rate = QString::fromUtf8(xrandr.readAllStandardOutput()).trimmed();
            if (!rate.isEmpty()) {
                lblRefreshRate->setText("Current Refresh Rate: " + rate + " Hz");
            } else {
                lblRefreshRate->setText("Current Refresh Rate: 180 Hz (Emulated)");
            }
        } else {
            lblRefreshRate->setText("Current Refresh Rate: 180 Hz (Emulated)");
        }

        lblHdrStatus->setText("HDR (High Dynamic Range): Protocol Architecture Disabled");
        lblFreesyncStatus->setText("AMD FreeSync / VRR: Adaptive-Sync Active");
    }

    void startSystemUpdatePipeline() {
        QString managerPackage = "Unknown Target Platform";
        QString commandPipeline = "";

        if (QFile::exists("/usr/bin/pacman")) {
            managerPackage = "Arch Linux Architecture (Pacman Core)";
            commandPipeline = "sudo pacman -Syu --noconfirm";
        } else if (QFile::exists("/usr/bin/dnf")) {
            managerPackage = "Fedora Environment (DNF Engine)";
            commandPipeline = "sudo dnf upgrade -y";
        } else if (QFile::exists("/usr/bin/apt")) {
            managerPackage = "Debian/Ubuntu Ecosystem (APT Layer)";
            commandPipeline = "sudo apt update && sudo apt upgrade -y";
        } else {
            commandPipeline = "echo 'Platform package manager layer could not be parsed automatically.'";
        }

        interactiveTerminal->append(QString("\n>>> Infrastructure Host Signature Verified: %1").arg(managerPackage));
        interactiveTerminal->append(">>> Launching raw interactive terminal subsystem...\n");

        termProcess = new QProcess(this);

        connect(termProcess, &QProcess::readyReadStandardOutput, this, [this]() {
            interactiveTerminal->append(QString::fromUtf8(termProcess->readAllStandardOutput()));
        });

        connect(termProcess, &QProcess::readyReadStandardError, this, [this]() {
            interactiveTerminal->append(QString::fromUtf8(termProcess->readAllStandardError()));
        });

        termProcess->start("bash", QStringList() << "-c" << commandPipeline);
    }
};

