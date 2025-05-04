#include "challengemode.h"
#include "mainwindow.h"
#include "backgroundwidget.h"
#include <QVBoxLayout>
#include <QMessageBox>

ChallengeMode::ChallengeMode(QWidget *parent)
    : QWidget(parent)
      , m_currentLevel(0)
{
    setupUI();
}

// ... 其他代码保持不变 ...
void ChallengeMode::setupUI()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

            // 创建关卡选择界面
    m_levelSelectWidget = new QWidget(this);

    // 首先创建背景控件作为基础层
    m_backgroundWidget = new BackgroundWidget(m_levelSelectWidget);
    m_backgroundWidget->setBackground("bg_challenge");

    // 确保背景填充整个m_levelSelectWidget
    QVBoxLayout* levelWidgetLayout = new QVBoxLayout(m_levelSelectWidget);
    levelWidgetLayout->setContentsMargins(0, 0, 0, 0);
    levelWidgetLayout->setSpacing(0);
    levelWidgetLayout->addWidget(m_backgroundWidget);

    // 在背景上创建关卡选择界面
    createLevelSelectUI();

            // 创建游戏UI
    m_gameUI = new GameUI(this);
    m_gameUI->hide();

            // 添加组件到主布局
    mainLayout->addWidget(m_levelSelectWidget);
    mainLayout->addWidget(m_gameUI);

    auto buttonLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonLayout);
}

void ChallengeMode::createLevelSelectUI()
{
    // 在背景控件上创建布局，而不是在m_levelSelectWidget上
    auto layout = new QGridLayout(m_backgroundWidget);
    layout->setContentsMargins(20, 20, 20, 20);

            // 创建标题
    auto titleLabel = new QLabel("挑战模式 - 选择关卡");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel, 0, 0, 1, 3);

            // 创建9个关卡按钮
    for (int i = 0; i < 9; ++i) {
        auto btn = new QPushButton(QString("第%1关\n%2x%2").arg(i + 1).arg(i + 4));
        btn->setFixedSize(120, 120);
        btn->setProperty("level", i + 1);
        connect(btn, &QPushButton::clicked, this, &ChallengeMode::onLevelSelected);

        m_levelButtons.append(btn);
        layout->addWidget(btn, (i / 3) + 1, i % 3);
    }
}

void ChallengeMode::showLevelSelect()
{
    m_levelSelectWidget->show();
    m_gameUI->hide();
}

void ChallengeMode::showGame()
{
    m_levelSelectWidget->hide();
    m_gameUI->show();
}


void ChallengeMode::startLevel(int level)
{
    m_currentLevel = level;
    qDebug() << "Starting level:" << level;
    qDebug() << "Cleaning up game state...";

            // 获取关卡参数并初始化游戏
    int rows, columns, medicineTypes, initialOpenDrawers, randomOperations;
    getLevelParameters(level, rows, columns, medicineTypes,
                       initialOpenDrawers, randomOperations);

    auto gameManager = GameManager::instance();
    // 先清理当前游戏状态
    gameManager->cleanupCurrentGame();

    // 重置 GameUI
    m_gameUI->reset();

    gameManager->initializeGame(rows, columns, medicineTypes,
                                initialOpenDrawers, randomOperations);
    gameManager->setGameUI(m_gameUI);

    connect(gameManager, &GameManager::gameCompleted,
            this, &ChallengeMode::onGameCompleted, Qt::UniqueConnection);

    gameManager->startNewGame(level);

    showGame();
}

ChallengeMode::~ChallengeMode()
{
   // 析构函数实现
}

void ChallengeMode::onLevelSelected()
{
    auto btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        int level = btn->property("level").toInt();
        startLevel(level);
    }
}

void ChallengeMode::onGameCompleted(int score, int steps)
{
    QString message = QString("恭喜通关！\n得分：%1\n步数：%2").arg(score).arg(steps);
    QMessageBox::information(this, "关卡完成", message);

    showLevelSelect();
}

void ChallengeMode::getLevelParameters(int level, int& rows, int& columns,
                                       int& medicineTypes, int& initialOpenDrawers,
                                       int& randomOperations)
{
    // 根据关卡设置参数
    rows = columns = level + 3;  // 4x4 到 12x12
    medicineTypes = rows*1.4;  // 药材种类随关卡增加
    initialOpenDrawers = (rows * columns) / 4;  // 初始打开1/4的抽屉
    randomOperations = level + 2;  // 随机操作次数随关卡增加
}
