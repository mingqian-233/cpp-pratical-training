

#include "mainwindow.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include "qtimer.h"
#include "setting.h"
#include "saveselectiondialog.h"

    MainWindow* MainWindow::m_instance = nullptr;

MainWindow* MainWindow::instance()
{
    if (!m_instance) {
        m_instance = new MainWindow();
    }
    return m_instance;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    QTimer::singleShot(0, this, &MainWindow::showSaveSelectDialog);

}
void MainWindow::showSaveSelectDialog()
{
    SaveSelectDialog* dialog = new SaveSelectDialog(this);
    if (dialog->exec() == QDialog::Accepted) {
        // 获取选择的存档
        m_saveManager = dialog->getSaveManager();

        // 如果成功加载存档，可以在这里更新UI或进行其他初始化
        if (m_saveManager) {
            QString playerName = m_saveManager->getPlayerName();
            setWindowTitle(QString("本草华章：我是小药童 - %1").arg(playerName));
        }
    } else {
        // 如果用户取消选择存档，可以选择退出游戏
        QMessageBox::critical(this, "错误", "必须选择或创建存档才能开始游戏！");
        QApplication::quit();
    }

    dialog->deleteLater();
}
MainWindow::~MainWindow()
{
    m_instance = nullptr;
}
void MainWindow::switchToMainPage()
{
    m_stackedWidget->setCurrentWidget(m_centralWidget);
}
void MainWindow::setupUI()
{
    // 设置窗口属性
    setWindowTitle("本草华章：我是小药童");
    resize(1280, 720);

            // 创建QStackedWidget作为中央窗口部件
    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);

            // 创建主页面
    m_centralWidget = new QWidget();

    // 创建并设置背景
    m_backgroundWidget = new BackgroundWidget(m_centralWidget);
    m_backgroundWidget->setBackground("bg_main");

            // 使用QVBoxLayout填充整个主页面
    QVBoxLayout* centralLayout = new QVBoxLayout(m_centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->addWidget(m_backgroundWidget);

            // 创建一个容器用于放置按钮
    QWidget* buttonContainer = new QWidget(m_backgroundWidget);
    m_mainLayout = new QVBoxLayout(buttonContainer);
    m_mainLayout->setAlignment(Qt::AlignCenter);
    m_mainLayout->setSpacing(20);

            // 创建按钮
    m_storyModeBtn = createMenuButton("剧情模式");
    m_challengeModeBtn = createMenuButton("挑战模式");
    m_customModeBtn = createMenuButton("自定义模式");
    m_settingsBtn = createMenuButton("设置");
    m_exitBtn = createMenuButton("退出");

            // 添加按钮到布局
    m_mainLayout->addWidget(m_storyModeBtn);
    m_mainLayout->addWidget(m_challengeModeBtn);
    m_mainLayout->addWidget(m_customModeBtn);
    m_mainLayout->addWidget(m_settingsBtn);
    m_mainLayout->addWidget(m_exitBtn);

            // 创建一个布局让按钮容器居中
    QVBoxLayout* bgLayout = new QVBoxLayout(m_backgroundWidget);
    bgLayout->addWidget(buttonContainer, 0, Qt::AlignCenter);

            // 创建挑战模式页面
    m_challengeMode = new ChallengeMode();

            // 将页面添加到QStackedWidget
    m_stackedWidget->addWidget(m_centralWidget);    // 主页面
    m_stackedWidget->addWidget(m_challengeMode);    // 挑战模式页面

            // 连接信号和槽
    connect(m_storyModeBtn, &QPushButton::clicked, this, &MainWindow::onStoryModeClicked);
    connect(m_challengeModeBtn, &QPushButton::clicked, this, &MainWindow::onChallengeModeClicked);
    connect(m_customModeBtn, &QPushButton::clicked, this, &MainWindow::onCustomModeClicked);
    connect(m_settingsBtn, &QPushButton::clicked, this, &MainWindow::onSettingsClicked);
    connect(m_exitBtn, &QPushButton::clicked, this, &MainWindow::onExitClicked);
    connect(m_challengeMode, &ChallengeMode::returnToMainMenuRequested,
            this, &MainWindow::switchToMainPage);

}

QPushButton* MainWindow::createMenuButton(const QString& text)
{
    QPushButton* button = new QPushButton(text, this);
    button->setMinimumSize(200, 60);
    button->setFont(QFont("Microsoft YaHei", 12, QFont::Bold));

            // 设置按钮样式
    button->setStyleSheet(
        "QPushButton {"
        "    background-color: rgba(255, 255, 255, 200);"
        "    border: 2px solid #8B4513;"
        "    border-radius: 10px;"
        "    color: #8B4513;"
        "    padding: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(139, 69, 19, 200);"
        "    color: white;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #654321;"
        "    border: 2px solid #654321;"
        "}"
        );

    return button;
}

void MainWindow::onStoryModeClicked()
{
    QMessageBox::information(this, "提示", "剧情模式正在开发中...");
}

void MainWindow::onChallengeModeClicked()
{
    QMessageBox::information(this, "提示", "挑战模式正在开发中...");
    m_stackedWidget->setCurrentWidget(m_challengeMode);

}

void MainWindow::onCustomModeClicked()
{
    QMessageBox::information(this, "提示", "自定义模式正在开发中...");
}

void MainWindow::onSettingsClicked()
{
    Setting::instance()->show();
}

void MainWindow::onExitClicked()
{
    QApplication::quit();
}
