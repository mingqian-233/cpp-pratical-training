#include "mainmenu.h"
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QDebug>

MainMenu::MainMenu(QWidget *parent)
    : QWidget(parent)
{
    // 创建背景控件（注意：不要将this作为parent）
    backgroundWidget = new BackgroundWidget(this);

    // 设置背景控件的尺寸
    backgroundWidget->setGeometry(0, 0, width(), height());

    // 将背景控件移到最底层
    backgroundWidget->lower();

    // 设置背景图片（不要加.png扩展名）
    backgroundWidget->setBackground("bg_main");

    setupUI();

    // 设置一个合适的初始窗口大小
    resize(800, 600);
}

MainMenu::~MainMenu()
{
   // Qt的父子对象系统会自动清理子控件，不需要手动删除
}
void MainMenu::setupUI()
{
    // 设置窗口标题
    setWindowTitle("Game Menu");

    // 创建一个透明的容器来放置按钮
    QWidget* container = new QWidget(this);
    container->setObjectName("buttonContainer");
    // 设置容器为透明，否则会遮挡背景
    container->setAttribute(Qt::WA_TranslucentBackground);

    // 创建按钮并设置固定大小
    QPushButton* storyModeButton = new QPushButton("剧情模式", container);
    QPushButton* challengeModeButton = new QPushButton("挑战模式", container);
    QPushButton* customModeButton = new QPushButton("自定义模式", container);
    QPushButton* settingsButton = new QPushButton("设置", container);
    QPushButton* exitButton = new QPushButton("退出", container);

    // 设置按钮大小策略，使其可以随窗口变化
    QList<QPushButton*> buttons = {storyModeButton, challengeModeButton,
                                   customModeButton, settingsButton, exitButton};

    // 为每个按钮设置大小和样式
    for (QPushButton* btn : buttons) {
        // 设置最小大小，确保按钮不会太小
        btn->setMinimumSize(150, 40);
        // 设置大小策略，使按钮可以随窗口变化
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        // 可以设置样式表使按钮更美观
        btn->setStyleSheet("QPushButton {"
            "background-color: rgba(255, 255, 255, 180);"
            "border: 2px solid #8f8f91;"
            "border-radius: 6px;"
            "font-size: 16px;"
            "padding: 6px;"
            "}"
            "QPushButton:hover {"
            "background-color: rgba(255, 255, 255, 220);"
            "border: 2px solid #3daee9;"
            "}");
    }

    // 创建按钮布局
    QVBoxLayout* buttonLayout = new QVBoxLayout(container);
    buttonLayout->addStretch(2); // 顶部弹性空间（更多）
    buttonLayout->addWidget(storyModeButton);
    buttonLayout->addWidget(challengeModeButton);
    buttonLayout->addWidget(customModeButton);
    buttonLayout->addWidget(settingsButton);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addStretch(2); // 底部弹性空间（更多）
    buttonLayout->setSpacing(20);
    buttonLayout->setAlignment(Qt::AlignCenter);

    // 设置按钮在布局中的对齐方式
    for (QPushButton* btn : buttons) {
        buttonLayout->setAlignment(btn, Qt::AlignHCenter);
    }

    // 创建主布局并将容器放在中央
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 移除边距
    mainLayout->addWidget(container);

    // 连接按钮信号
    connect(storyModeButton, &QPushButton::clicked, this, &MainMenu::onStoryModeClicked);
    connect(challengeModeButton, &QPushButton::clicked, this, &MainMenu::onChallengeModeClicked);
    connect(customModeButton, &QPushButton::clicked, this, &MainMenu::onCustomModeClicked);
    connect(settingsButton, &QPushButton::clicked, this, &MainMenu::onSettingsClicked);
    connect(exitButton, &QPushButton::clicked, this, &MainMenu::onExitClicked);
}

void MainMenu::resizeEvent(QResizeEvent *event)
{
    // 当窗口大小变化时，更新背景控件的大小
    backgroundWidget->setGeometry(0, 0, width(), height());
    QWidget::resizeEvent(event);
}

// 以下是按钮点击事件处理函数（保持不变）
void MainMenu::onStoryModeClicked()
{
   // 剧情模式处理代码
}

void MainMenu::onChallengeModeClicked()
{
   // 挑战模式处理代码
}

void MainMenu::onCustomModeClicked()
{
   // 自定义模式处理代码
}

void MainMenu::onSettingsClicked()
{
   // 设置处理代码
}

void MainMenu::onExitClicked()
{
    // 退出程序
    QApplication::quit();
}
