#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QRegularExpression>
#include <QPainter>
#include <QImage>

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
                                  "QScrollBar::handle:vertical { background: #303030; border-radius: 4px; }"
                                  "QScrollBar::handle:vertical:hover { background: #404040; }");

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
        scanBtn->setStyleSheet("QPushButton { background-color: #1A1A1A; border: 1px solid #FF002B; color: #FFFFFF; font-weight: bold; border-radius: 4px; font-size: 12px; }"
                               "QPushButton:hover { background-color: #251015; }");
        connect(scanBtn, &QPushButton::clicked, this, &GamingTab::scanSteamGames);

        QPushButton *clearCacheBtn = new QPushButton(" Clear Shader Cache", this);
        clearCacheBtn->setFixedHeight(36);
        clearCacheBtn->setStyleSheet("QPushButton { background-color: #FF002B; border: none; color: #FFFFFF; font-weight: bold; border-radius: 4px; font-size: 12px; }"
                                     "QPushButton:hover { background-color: #E00024; }");
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
            QDir dir(path);
            if (dir.exists()) {
                QProcess::execute("rm", QStringList() << "-rf" << path);
            }
        }
    }

    void scanSteamGames() {
        QLayoutItem *item;
        while ((item = gamesGrid->takeAt(0)) != nullptr) {
            if (item->widget()) delete item->widget();
            delete item;
        }

        QStringList steamPaths = {
            QDir::homePath() + "/.steam/steam/steamapps",
            QDir::homePath() + "/.local/share/Steam/steamapps"
        };

        int row = 0, col = 0;

        for (const QString &path : steamPaths) {
            QDir dir(path);
            if (!dir.exists()) continue;

            QStringList filters;
            filters << "appmanifest_*.acf";
            QStringList files = dir.entryList(filters, QDir::Files);

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
                        if (match.hasMatch()) {
                            gameName = match.captured(1);
                            break;
                        }
                    }
                }
                f.close();

                if (!gameName.isEmpty()) {
                    QWidget *card = createGameCard(gameName);
                    gamesGrid->addWidget(card, row, col);
                    col++;
                    if (col >= 3) {
                        col = 0;
                        row++;
                    }
                }
            }
        }
    }

    QWidget* createGameCard(const QString &name) {
        QWidget *card = new QWidget(this);
        card->setStyleSheet("QWidget { background-color: #1A1A1A; border: 1px solid #2D2D2D; border-radius: 4px; }"
                            "QWidget:hover { border: 1px solid #FF002B; }");
        card->setFixedHeight(100);

        QVBoxLayout *layout = new QVBoxLayout(card);
        layout->setContentsMargins(15, 12, 15, 12);
        layout->setSpacing(8);

        QLabel *gameTitle = new QLabel(name, card);
        gameTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #FFFFFF; border: none;");
        gameTitle->setWordWrap(true);

        QPushButton *optBtn = new QPushButton("Tune Profiles", card);
        optBtn->setFixedHeight(28);
        optBtn->setStyleSheet("QPushButton { background-color: transparent; border: 1px solid #FF002B; color: #FF002B; font-weight: bold; border-radius: 3px; font-size: 11px; }"
                              "QPushButton:hover { background-color: #FF002B; color: #FFFFFF; }");

        layout->addWidget(gameTitle);
        layout->addStretch();
        layout->addWidget(optBtn);

        return card;
    }
};
