

#include "mainwindow.h"

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
        // 不需要获取SaveManager，直接使用单例
        m_saveManager = SaveManager::instance();

                // 如果成功加载存档，可以在这里更新UI或进行其他初始化
        QString playerName = m_saveManager->getPlayerName();
        setWindowTitle(QString("本草华章：我是小药童 - %1").arg(playerName));
    }else {
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

            // 将页面添加到QStackedWidget
    m_stackedWidget->addWidget(m_centralWidget);    // 主页面
            // 连接信号和槽
    connect(m_storyModeBtn, &QPushButton::clicked, this, &MainWindow::onStoryModeClicked);
    connect(m_challengeModeBtn, &QPushButton::clicked, this, &MainWindow::onChallengeModeClicked);
    connect(m_customModeBtn, &QPushButton::clicked, this, &MainWindow::onCustomModeClicked);
    connect(m_settingsBtn, &QPushButton::clicked, this, &MainWindow::onSettingsClicked);
    connect(m_exitBtn, &QPushButton::clicked, this, &MainWindow::onExitClicked);

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

// 添加一个新的成员函数来处理章节加载
void MainWindow::loadChapter(int chapterNumber) {
    // 获取当前剧情进度
    SaveManager* saveManager = m_saveManager;

    // 创建故事模式页面
    QWidget* storyPage = new QWidget();
    m_stackedWidget->addWidget(storyPage);

            // 创建布局
    QVBoxLayout* storyLayout = new QVBoxLayout(storyPage);
    storyLayout->setContentsMargins(0, 0, 0, 0);

            // 创建故事模式实例
    StoryMode* storyMode = new StoryMode(storyPage);

            // 添加到布局
    storyLayout->addWidget(storyMode);

            // 切换到故事模式页面
    m_stackedWidget->setCurrentWidget(storyPage);

            // 加载当前章节
    if (!storyMode->loadChapter(chapterNumber)) {
        // 加载失败，返回主菜单
        QMessageBox::warning(this, "提示", "已完成全部章节！");
        m_stackedWidget->setCurrentWidget(m_centralWidget);
        storyPage->deleteLater();
        return;
    }

            // 连接故事完成信号
    connect(storyMode, &StoryMode::storyCompleted, this, [this, storyPage, chapterNumber, saveManager](QJsonObject gameParams) {
        // 创建游戏页面
        QWidget* gamePage = new QWidget();
        m_stackedWidget->addWidget(gamePage);

                // 获取游戏参数
        int medicineTypes = gameParams["medicineTypes"].toInt(5);
        int rows = gameParams["rows"].toInt(3);
        int cols = gameParams["cols"].toInt(3);
        int operationCount = gameParams["operationCount"].toInt(3);

                // 创建布局
        QVBoxLayout* gameLayout = new QVBoxLayout(gamePage);
        gameLayout->setContentsMargins(0, 0, 0, 0);

                // 创建游戏实例
        MedicineGame* game = new MedicineGame(gamePage);
        game->initGame(medicineTypes, rows, cols, operationCount);

                // 添加到布局
        gameLayout->addWidget(game);

                // 切换到游戏页面
        m_stackedWidget->setCurrentWidget(gamePage);

                // 设置游戏完成时的回调
        connect(game, &MedicineGame::gameCompleted, this, [this, gamePage, storyPage, chapterNumber, saveManager]() {
            // 更新故事进度
            saveManager->setStoryProgress(chapterNumber);  // 保存当前完成的章节编号
            saveManager->saveSaveFile();
            saveManager->saveSaveFile(saveManager->getCurrentSaveFile());
            qDebug() << "已保存游戏进度：章节" << chapterNumber;


                    // 创建自定义对话框
            QDialog* nextChapterDialog = new QDialog(this);
            nextChapterDialog->setWindowTitle("章节完成");
            nextChapterDialog->setFixedSize(400, 300);
            nextChapterDialog->setStyleSheet("QDialog { border-image: url(qrc:/images/next_chapter.png) stretch; }");

            QVBoxLayout* dialogLayout = new QVBoxLayout(nextChapterDialog);
            QLabel* messageLabel = new QLabel("恭喜完成当前章节！是否继续下一章？", nextChapterDialog);
            messageLabel->setStyleSheet("QLabel { color: white; font-size: 18px; background-color: rgba(0, 0, 0, 100); padding: 10px; border-radius: 5px; }");
            messageLabel->setAlignment(Qt::AlignCenter);

            QHBoxLayout* buttonLayout = new QHBoxLayout();
            QPushButton* nextButton = new QPushButton("继续下一章", nextChapterDialog);
            QPushButton* returnButton = new QPushButton("返回主菜单", nextChapterDialog);

            nextButton->setMinimumHeight(40);
            returnButton->setMinimumHeight(40);
            nextButton->setStyleSheet("QPushButton { font-size: 16px; background-color: #4CAF50; color: white; border-radius: 5px; }"
                "QPushButton:hover { background-color: #45a049; }");
            returnButton->setStyleSheet("QPushButton { font-size: 16px; background-color: #f44336; color: white; border-radius: 5px; }"
                "QPushButton:hover { background-color: #d32f2f; }");

            buttonLayout->addWidget(nextButton);
            buttonLayout->addWidget(returnButton);

            dialogLayout->addStretch();
            dialogLayout->addWidget(messageLabel);
            dialogLayout->addStretch();
            dialogLayout->addLayout(buttonLayout);
            dialogLayout->addStretch();

            // 返回主菜单按钮的点击事件
            connect(returnButton, &QPushButton::clicked, [this, nextChapterDialog, gamePage, storyPage, chapterNumber]() {
                nextChapterDialog->accept();

                // 显示进度已保存的提示
                QMessageBox::information(this, "游戏进度", QString("已保存至第%1章").arg(chapterNumber));

                // 切换回主菜单
                m_stackedWidget->setCurrentWidget(m_centralWidget);

                // 恢复主菜单音乐
                MusicManager::instance()->switchMusic("taqing.mp3");

                // 删除游戏和故事页面
                m_stackedWidget->removeWidget(gamePage);
                gamePage->deleteLater();

                m_stackedWidget->removeWidget(storyPage);
                storyPage->deleteLater();
            });

            // 继续下一章按钮的点击事件
            connect(nextButton, &QPushButton::clicked, [this, nextChapterDialog, gamePage, storyPage, chapterNumber, &saveManager]() {
                nextChapterDialog->accept();

                // 删除当前游戏和故事页面
                m_stackedWidget->removeWidget(gamePage);
                gamePage->deleteLater();
                m_stackedWidget->removeWidget(storyPage);
                storyPage->deleteLater();

                // 加载下一章节 - 调用loadChapter函数
                this->loadChapter(chapterNumber + 1);
            });

            // 显示对话框
            nextChapterDialog->exec();
            nextChapterDialog->deleteLater();
        });
    });

            // 连接返回主菜单的信号
    connect(storyMode, &StoryMode::returnToMainMenu, this, [this, storyPage]() {
        m_stackedWidget->setCurrentWidget(m_centralWidget);

                // 恢复主菜单音乐
        MusicManager::instance()->switchMusic("taqing.mp3");

                // 删除故事页面
        m_stackedWidget->removeWidget(storyPage);
        storyPage->deleteLater();
    });
}
void MainWindow::onStoryModeClicked()
{
    // 获取当前剧情进度
    SaveManager* saveManager = m_saveManager;
    int currentChapter = saveManager->getStoryProgress();
    qDebug() << currentChapter;

            // 调用loadChapter函数加载章节
    if(currentChapter==6){
        // 创建自定义对话框
        QDialog* resetDialog = new QDialog(this);
        resetDialog->setWindowTitle("剧情模式完成");
        resetDialog->setFixedSize(400, 250);
        resetDialog->setStyleSheet("QDialog { background-color: #f5f5f5; border: 2px solid #8a6d3b; }");

        QVBoxLayout* dialogLayout = new QVBoxLayout(resetDialog);
        QLabel* messageLabel = new QLabel("恭喜您已完成所有剧情！\n是否重置剧情模式从头开始？", resetDialog);
        messageLabel->setStyleSheet("QLabel { color: #8a6d3b; font-size: 18px; font-weight: bold; padding: 10px; }");
        messageLabel->setAlignment(Qt::AlignCenter);

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        QPushButton* resetButton = new QPushButton("重新开始", resetDialog);
        QPushButton* returnButton = new QPushButton("返回主菜单", resetDialog);

        resetButton->setMinimumHeight(40);
        returnButton->setMinimumHeight(40);
        resetButton->setStyleSheet("QPushButton { font-size: 16px; background-color: #4CAF50; color: white; border-radius: 5px; }"
            "QPushButton:hover { background-color: #45a049; }");
        returnButton->setStyleSheet("QPushButton { font-size: 16px; background-color: #f44336; color: white; border-radius: 5px; }"
            "QPushButton:hover { background-color: #d32f2f; }");

        buttonLayout->addWidget(resetButton);
        buttonLayout->addWidget(returnButton);

        dialogLayout->addStretch();
        dialogLayout->addWidget(messageLabel);
        dialogLayout->addStretch();
        dialogLayout->addLayout(buttonLayout);
        dialogLayout->addStretch();

        // 重置按钮的点击事件
        connect(resetButton, &QPushButton::clicked, [this, resetDialog, saveManager]() {
            resetDialog->accept();

            // 重置进度
            saveManager->setStoryProgress(0);
            saveManager->saveSaveFile();
            qDebug() << "剧情进度已重置";

            // 从第一章开始
            loadChapter(0 + 1);
        });

        // 返回主菜单按钮的点击事件
        connect(returnButton, &QPushButton::clicked, [this, resetDialog]() {
            resetDialog->accept();

            // 切换回主菜单（不重置进度）
            m_stackedWidget->setCurrentWidget(m_centralWidget);
        });

        // 显示对话框
        resetDialog->exec();
        resetDialog->deleteLater();
    }
    else loadChapter(currentChapter + 1);
}

// 修改 onChallengeModeClicked 函数
void MainWindow::onChallengeModeClicked()
{
    // 创建挑战模式选关界面
    QWidget* levelSelectWidget = new QWidget();
    m_stackedWidget->addWidget(levelSelectWidget);

    // Initialize music manager
    MusicManager::instance()->switchMusic("challenge.mp3");
    // 创建背景
    BackgroundWidget* levelBackground = new BackgroundWidget(levelSelectWidget);
    levelBackground->setBackground("bg_challenge");

    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(levelSelectWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 创建标题
    QLabel* titleLabel = new QLabel("挑战模式 - 选择难度");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: white; background-color: rgba(0, 0, 0, 150); padding: 10px; border-radius: 10px;");

    // 创建关卡选择网格
    QGridLayout* levelGrid = new QGridLayout();
    levelGrid->setSpacing(20);

    // 创建9个关卡按钮
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            int level = row * 3 + col + 1;
            QPushButton* levelButton = new QPushButton(QString::number(level));
            levelButton->setMinimumSize(100, 100);
            levelButton->setFont(QFont("Microsoft YaHei", 18, QFont::Bold));
            levelButton->setStyleSheet(
                "QPushButton {"
                "    background-color: rgba(255, 255, 255, 180);"
                "    border: 3px solid #8B4513;"
                "    border-radius: 15px;"
                "    color: #8B4513;"
                "}"
                "QPushButton:hover {"
                "    background-color: rgba(139, 69, 19, 180);"
                "    color: white;"
                "}"
                "QPushButton:pressed {"
                "    background-color: #654321;"
                "    border: 3px solid #654321;"
                "}"
                );

            // 连接按钮信号到启动关卡的槽
            connect(levelButton, &QPushButton::clicked, this, [this, level]() {
                startChallengeLevel(level);
            });

            levelGrid->addWidget(levelButton, row, col);
        }
    }

    // 创建返回按钮
    QPushButton* backButton = new QPushButton("返回主菜单");
    backButton->setMinimumSize(150, 50);
    backButton->setFont(QFont("Microsoft YaHei", 12, QFont::Bold));
    backButton->setStyleSheet(
        "QPushButton {"
        "    background-color: rgba(255, 255, 255, 180);"
        "    border: 2px solid #8B4513;"
        "    border-radius: 10px;"
        "    color: #8B4513;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(139, 69, 19, 180);"
        "    color: white;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #654321;"
        "    border: 2px solid #654321;"
        "}"
        );

    // 连接返回按钮
    connect(backButton, &QPushButton::clicked, this, [this, levelSelectWidget]() {
        m_stackedWidget->setCurrentWidget(m_centralWidget);
        // 延迟删除关卡选择界面
        levelSelectWidget->deleteLater();

        MusicManager::instance()->switchMusic("taqing.mp3");
    });

    // 创建一个容器来放置关卡网格
    QWidget* gridContainer = new QWidget();
    gridContainer->setLayout(levelGrid);
    gridContainer->setStyleSheet("background-color: rgba(255, 235, 205, 150); border-radius: 20px; padding: 20px;");

    // 将所有元素添加到主布局
    QVBoxLayout* contentLayout = new QVBoxLayout();
    contentLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    contentLayout->addSpacing(20);
    contentLayout->addWidget(gridContainer, 0, Qt::AlignCenter);
    contentLayout->addSpacing(20);
    contentLayout->addWidget(backButton, 0, Qt::AlignCenter);
    contentLayout->setAlignment(Qt::AlignCenter);

    QWidget* contentWidget = new QWidget();
    contentWidget->setLayout(contentLayout);

    QVBoxLayout* bgLayout = new QVBoxLayout(levelBackground);
    bgLayout->addWidget(contentWidget, 0, Qt::AlignCenter);

    mainLayout->addWidget(levelBackground);

    // 切换到关卡选择界面
    m_stackedWidget->setCurrentWidget(levelSelectWidget);
}

// 添加到MainWindow类的私有方法声明中
void MainWindow::startChallengeLevel(int level)
{
    // 根据难度级别计算游戏参数
    int medicineTypes = 5 + level;         // 药材种类数随难度增加
    int rows = 2 + (level - 1) / 3;        // 行数：难度1-3为2行，4-6为3行，7-9为4行
    int cols = 2 + (level - 1) % 3;        // 列数：难度1,4,7为2列，2,5,8为3列，3,6,9为4列
    int operationCount = level * 1.5;        // 随机操作次数随难度增加

    // 创建游戏页面
    QWidget* gamePage = new QWidget();
    m_stackedWidget->addWidget(gamePage);

    // 创建布局
    QVBoxLayout* gameLayout = new QVBoxLayout(gamePage);
    gameLayout->setContentsMargins(0, 0, 0, 0);

    // 创建游戏实例
    MedicineGame* game = new MedicineGame(gamePage);
    game->initGame(medicineTypes, rows, cols, operationCount);

    // 添加到布局
    gameLayout->addWidget(game);

    // 切换到游戏页面
    m_stackedWidget->setCurrentWidget(gamePage);

    // 设置游戏完成时的回调
    connect(game, &MedicineGame::gameCompleted, this, [this, gamePage]() {
        // 延迟一段时间后返回选关界面
        QTimer::singleShot(2000, this, [this, gamePage]() {
            // 获取选关界面
            QWidget* levelSelectWidget = m_stackedWidget->widget(m_stackedWidget->count() - 2);

            // 切换回选关界面
            m_stackedWidget->setCurrentWidget(levelSelectWidget);

            // 删除游戏页面
            m_stackedWidget->removeWidget(gamePage);
            gamePage->deleteLater();

        });
    });
}


// ... existing code ...
void MainWindow::onCustomModeClicked()
{
    // 创建选择界面：新建自定义/读取自定义
    QDialog* customDialog = new QDialog(this);
    customDialog->setWindowTitle("自定义模式");
    customDialog->setMinimumSize(300, 200);

    QVBoxLayout* dialogLayout = new QVBoxLayout(customDialog);

    // 创建两个按钮
    QPushButton* newCustomBtn = createMenuButton("新建自定义");
    QPushButton* loadCustomBtn = createMenuButton("读取自定义");

    dialogLayout->addWidget(new QLabel("请选择操作："), 0, Qt::AlignCenter);
    dialogLayout->addSpacing(20);
    dialogLayout->addWidget(newCustomBtn, 0, Qt::AlignCenter);
    dialogLayout->addWidget(loadCustomBtn, 0, Qt::AlignCenter);

    // 连接按钮信号
    connect(newCustomBtn, &QPushButton::clicked, [this, customDialog]() {
        customDialog->accept();
        createCustomLevel();
    });

    connect(loadCustomBtn, &QPushButton::clicked, [this, customDialog]() {
        customDialog->accept();
        loadCustomLevel();
    });

    customDialog->exec();
    customDialog->deleteLater();
}

void MainWindow::createCustomLevel()
{
    // 创建新建自定义界面
    QDialog* createDialog = new QDialog(this);
    createDialog->setWindowTitle("新建自定义关卡");
    createDialog->setMinimumSize(400, 500);

    QVBoxLayout* mainLayout = new QVBoxLayout(createDialog);

    // 药柜行列设置
    QGroupBox* gridGroup = new QGroupBox("药柜设置");
    QHBoxLayout* gridLayout = new QHBoxLayout(gridGroup);

    QSpinBox* rowsSpin = new QSpinBox();
    rowsSpin->setRange(2, 8);
    rowsSpin->setValue(3);
    rowsSpin->setPrefix("行数: ");

    QSpinBox* colsSpin = new QSpinBox();
    colsSpin->setRange(2, 8);
    colsSpin->setValue(3);
    colsSpin->setPrefix("列数: ");

    gridLayout->addWidget(rowsSpin);
    gridLayout->addWidget(colsSpin);

    // 药材选择
    QGroupBox* medicineGroup = new QGroupBox("药材选择");
    QVBoxLayout* medicineLayout = new QVBoxLayout(medicineGroup);

    QListWidget* medicineList = new QListWidget();
    medicineList->setSelectionMode(QAbstractItemView::MultiSelection);

    // 从文件加载药材名称
    QFile file(":/data/medicine_name.txt");
    QStringList allMedicines;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) {
                allMedicines.append(line);
                QListWidgetItem* item = new QListWidgetItem(line, medicineList);
                item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                item->setCheckState(Qt::Unchecked);
            }
        }
        file.close();
    } else {
        // 默认药材列表
        allMedicines = QStringList() << "人参" << "当归" << "黄芪" << "甘草" << "茯苓"
                                     << "白术" << "川芎" << "熟地" << "干姜" << "陈皮"
                                     << "枸杞" << "菊花" << "红花" << "薄荷" << "桂枝";

        for (const QString& med : allMedicines) {
            QListWidgetItem* item = new QListWidgetItem(med, medicineList);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Unchecked);
        }
    }

    medicineLayout->addWidget(medicineList);

    // 确认按钮
    QPushButton* confirmBtn = createMenuButton("确认");

    // 添加组件到主布局
    mainLayout->addWidget(gridGroup);
    mainLayout->addWidget(medicineGroup);
    mainLayout->addWidget(confirmBtn, 0, Qt::AlignCenter);

    // 连接确认按钮
    connect(confirmBtn, &QPushButton::clicked, [this, createDialog, rowsSpin, colsSpin, medicineList, allMedicines]() {
        int rows = rowsSpin->value();
        int cols = colsSpin->value();

        // 获取选中的药材
        QStringList selectedMedicines;
        for (int i = 0; i < medicineList->count(); i++) {
            QListWidgetItem* item = medicineList->item(i);
            if (item->checkState() == Qt::Checked) {
                selectedMedicines.append(item->text());
            }
        }

        // 至少需要选择两种药材
        if (selectedMedicines.size() < 2) {
            QMessageBox::warning(createDialog, "警告", "请至少选择两种药材！");
            return;
        }

        createDialog->accept();
        setupCustomDrawers(rows, cols, selectedMedicines);
    });

    createDialog->exec();
    createDialog->deleteLater();
}

void MainWindow::setupCustomDrawers(int rows, int cols, const QStringList& medicines)
{
    // 创建药柜设置界面
    QWidget* drawerSetupWidget = new QWidget();
    m_stackedWidget->addWidget(drawerSetupWidget);

    // 创建背景
    BackgroundWidget* bg = new BackgroundWidget(drawerSetupWidget);
    bg->setBackground("bg_custom");

    QVBoxLayout* mainLayout = new QVBoxLayout(drawerSetupWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(bg);

    // 创建内容布局
    QHBoxLayout* contentLayout = new QHBoxLayout();

    // 左侧药材列表
    QGroupBox* medicineGroup = new QGroupBox("可用药材");
    QVBoxLayout* medicineLayout = new QVBoxLayout(medicineGroup);
    QListWidget* medicineListWidget = new QListWidget();
    for (const QString& med : medicines) {
        new QListWidgetItem(med, medicineListWidget);
    }
    medicineLayout->addWidget(medicineListWidget);

    // 右侧药柜设置
    QGroupBox* drawerGroup = new QGroupBox("药柜设置");
    QVBoxLayout* drawerLayoutContainer = new QVBoxLayout(drawerGroup);
    QGridLayout* drawersGrid = new QGridLayout();
    drawersGrid->setSpacing(10);

    // 储存药柜和抽屉状态的数据结构
    QVector<QVector<QPushButton*>> drawerButtons;
    QVector<QVector<QComboBox*>> leftComboBoxes;
    QVector<QVector<QComboBox*>> rightComboBoxes;
    QVector<QVector<bool>> drawerStates;

    // 创建药柜网格
    for (int r = 0; r < rows; r++) {
        QVector<QPushButton*> rowButtons;
        QVector<QComboBox*> rowLeftCombos;
        QVector<QComboBox*> rowRightCombos;
        QVector<bool> rowStates;

        for (int c = 0; c < cols; c++) {
            // 创建药柜容器
            QWidget* drawerWidget = new QWidget();
            QHBoxLayout* drawerLayout = new QHBoxLayout(drawerWidget);
            drawerLayout->setContentsMargins(5, 5, 5, 5);

            // 左侧药材选择
            QComboBox* leftCombo = new QComboBox();
            leftCombo->addItems(medicines);

            // 右侧药材选择
            QComboBox* rightCombo = new QComboBox();
            rightCombo->addItems(medicines);
            if (medicines.size() > 1) {
                rightCombo->setCurrentIndex(1); // 默认选择第二种药材
            }

            // 药柜按钮（显示开/关状态）
            QPushButton* drawerBtn = new QPushButton("关闭");
            drawerBtn->setCheckable(true);
            drawerBtn->setStyleSheet(
                "QPushButton { background-color: #f0d0a0; border: 2px solid #805030; border-radius: 5px; }"
                "QPushButton:checked { background-color: #80c080; }"
                );

            // 连接药柜按钮点击事件
            connect(drawerBtn, &QPushButton::toggled, [drawerBtn](bool checked) {
                drawerBtn->setText(checked ? "打开" : "关闭");
            });

            // 添加组件到布局
            drawerLayout->addWidget(leftCombo);
            drawerLayout->addWidget(drawerBtn);
            drawerLayout->addWidget(rightCombo);

            drawersGrid->addWidget(drawerWidget, r, c);

            // 保存到数据结构
            rowButtons.append(drawerBtn);
            rowLeftCombos.append(leftCombo);
            rowRightCombos.append(rightCombo);
            rowStates.append(false); // 默认关闭
        }

        drawerButtons.append(rowButtons);
        leftComboBoxes.append(rowLeftCombos);
        rightComboBoxes.append(rowRightCombos);
        drawerStates.append(rowStates);
    }

    drawerLayoutContainer->addLayout(drawersGrid);

    // 创建保存按钮
    QPushButton* saveBtn = createMenuButton("储存关卡");
    QPushButton* backBtn = createMenuButton("返回");

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(backBtn);
    btnLayout->addWidget(saveBtn);

    drawerLayoutContainer->addLayout(btnLayout);

    // 添加组件到内容布局
    contentLayout->addWidget(medicineGroup, 1);
    contentLayout->addWidget(drawerGroup, 3);

    // 创建内容容器
    QWidget* contentWidget = new QWidget();
    contentWidget->setLayout(contentLayout);
    contentWidget->setStyleSheet("background-color: rgba(255, 255, 255, 200); border-radius: 10px;");

    // 将内容添加到背景
    QVBoxLayout* bgLayout = new QVBoxLayout(bg);
    bgLayout->addWidget(contentWidget);

    // 连接返回按钮
    connect(backBtn, &QPushButton::clicked, [this, drawerSetupWidget]() {
        m_stackedWidget->setCurrentWidget(m_centralWidget);
        drawerSetupWidget->deleteLater();
    });

    // 连接保存按钮
    connect(saveBtn, &QPushButton::clicked, [this, rows, cols, drawerButtons, leftComboBoxes, rightComboBoxes, medicines, drawerSetupWidget]() {
        // 创建保存数据
        QMap<QString, int> medicineList;
        QJsonArray drawersData;

        // 收集药柜数据
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                QPushButton* btn = drawerButtons[r][c];
                QString leftMed = leftComboBoxes[r][c]->currentText();
                QString rightMed = rightComboBoxes[r][c]->currentText();
                bool isOpen = btn->isChecked();

                // 如果抽屉是打开的，添加药材到清单
                if (isOpen) {
                    medicineList[leftMed] = medicineList.value(leftMed, 0) + 1;
                    medicineList[rightMed] = medicineList.value(rightMed, 0) + 1;
                }

                // 保存抽屉数据
                QJsonObject drawer;
                drawer["row"] = r;
                drawer["col"] = c;
                drawer["leftMedicine"] = leftMed;
                drawer["rightMedicine"] = rightMed;
                drawer["isOpen"] = isOpen;
                drawersData.append(drawer);
            }
        }

        // 创建JSON数据
        QJsonObject levelData;
        levelData["rows"] = rows;
        levelData["cols"] = cols;

        // 药材清单
        QJsonArray medicineListData;
        for (auto it = medicineList.begin(); it != medicineList.end(); ++it) {
            QJsonObject med;
            med["name"] = it.key();
            med["count"] = it.value();
            medicineListData.append(med);
        }

        levelData["medicineList"] = medicineListData;
        levelData["drawers"] = drawersData;

        // 确保目录存在
        QDir dir("./customlevel");
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        // 生成文件名
        QString fileName = QInputDialog::getText(
            drawerSetupWidget,
            "保存关卡",
            "请输入关卡名称:",
            QLineEdit::Normal,
            QString("自定义关卡_%1x%2").arg(rows).arg(cols)
            );

        if (fileName.isEmpty()) {
            return;
        }

        // 确保文件名合法
        fileName = fileName.replace(QRegularExpression("[\\\\/:*?\"<>|]"), "_");

        // 获取当前系统时间并格式化为字符串
        QDateTime currentTime = QDateTime::currentDateTime();
        QString timeStr = currentTime.toString("yyyyMMdd_HHmmss");

        // 将时间添加到文件名后面
        QString fileNameWithTime = QString("%1_%2").arg(fileName).arg(timeStr);
        QString filePath = QString("./customlevel/%1.json").arg(fileNameWithTime);

        // 保存到文件
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            QJsonDocument doc(levelData);
            file.write(doc.toJson());
            file.close();

            QMessageBox::information(drawerSetupWidget, "成功", "关卡已保存！");

            // 让玩家设计游戏
            designCustomGame(filePath);
        } else {
            QMessageBox::warning(drawerSetupWidget, "错误", "无法保存文件！");
        }
    });

    // 切换到药柜设置界面
    m_stackedWidget->setCurrentWidget(drawerSetupWidget);
}

void MainWindow::loadCustomLevel()
{
    // 确保目录存在
    QDir dir("./customlevel");
    if (!dir.exists() || dir.entryList(QStringList() << "*.json", QDir::Files).isEmpty()) {
        QMessageBox::information(this, "提示", "没有找到自定义关卡！请先创建一个关卡。");
        return;
    }

    // 获取所有自定义关卡文件
    QStringList fileNames = dir.entryList(QStringList() << "*.json", QDir::Files);

    // 选择对话框
    bool ok;
    QString selectedFile = QInputDialog::getItem(
        this,
        "选择关卡",
        "请选择要加载的关卡:",
        fileNames,
        0,
        false,
        &ok
        );

    if (!ok || selectedFile.isEmpty()) {
        return;
    }

    // 启动自定义游戏
    startCustomGame("./customlevel/" + selectedFile);
}

void MainWindow::startCustomGame(const QString& filePath)
{
    // 读取关卡文件
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法打开关卡文件！");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject levelData = doc.object();

    // 创建游戏页面
    QWidget* gamePage = new QWidget();
    m_stackedWidget->addWidget(gamePage);

    // 创建布局
    QVBoxLayout* gameLayout = new QVBoxLayout(gamePage);
    gameLayout->setContentsMargins(0, 0, 0, 0);

    // 创建游戏实例
    MedicineGame* game = new MedicineGame(gamePage);

    // 传递自定义关卡数据给游戏
    game->initCustomGame(levelData);

    // 添加到布局
    gameLayout->addWidget(game);



    // 切换到游戏页面
    MusicManager::instance()->switchMusic("custom.mp3");
    m_stackedWidget->setCurrentWidget(gamePage);

    // 设置游戏完成时的回调
    connect(game, &MedicineGame::gameCompleted, this, [this, gamePage]() {
        // 延迟一段时间后返回主菜单
        QTimer::singleShot(2000, this, [this, gamePage]() {
            // 切换回主菜单
            m_stackedWidget->setCurrentWidget(m_centralWidget);

            MusicManager::instance()->switchMusic("taqing.mp3");
            // 删除游戏页面

            m_stackedWidget->removeWidget(gamePage);
            gamePage->deleteLater();
        });
    });
}
void MainWindow::designCustomGame(const QString& filePath)
{
    // 读取关卡文件
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法打开关卡文件！");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject levelData = doc.object();

            // 创建设计页面
    QWidget* designPage = new QWidget();
    m_stackedWidget->addWidget(designPage);

    // 创建背景
    BackgroundWidget* bg = new BackgroundWidget(designPage);
    bg->setBackground("bg_custom");

    QVBoxLayout* mainLayout = new QVBoxLayout(designPage);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(bg);

    // 创建内容布局
    QVBoxLayout* contentLayout = new QVBoxLayout();

    // 创建标题
    QLabel* titleLabel = new QLabel("自定义关卡设计 - 打乱药柜");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #805030; background-color: rgba(255, 255, 255, 180); padding: 10px;");
    contentLayout->addWidget(titleLabel);

    // 创建游戏部分和操作记录部分的水平布局
    QHBoxLayout* gameOperationLayout = new QHBoxLayout();

    // 创建游戏实例
    MedicineGame* game = new MedicineGame(nullptr,true);
    game->designCustomGame(levelData);
    gameOperationLayout->addWidget(game, 3); // 游戏占3份空间

    // 创建操作记录部分
    QWidget* operationPanel = new QWidget();
    operationPanel->setStyleSheet("background-color: rgba(255, 255, 255, 150); border-radius: 10px; padding: 10px;");
    QVBoxLayout* operationLayout = new QVBoxLayout(operationPanel);

    QLabel* operationLabel = new QLabel("操作记录：");
    operationLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #805030;");
    operationLayout->addWidget(operationLabel);

    QListWidget* operationList = new QListWidget();
    operationList->setStyleSheet("background-color: rgba(255, 255, 255, 150); border: 1px solid #805030;");
    operationLayout->addWidget(operationList);

    QLabel* instructionLabel = new QLabel("请点击药材名称打乱药柜，完成后点击\"完成打乱\"按钮");
    instructionLabel->setAlignment(Qt::AlignCenter);
    instructionLabel->setStyleSheet("font-size: 14px; color: #805030; padding: 5px;");
    operationLayout->addWidget(instructionLabel);

    QPushButton* completeBtn = createMenuButton("完成打乱");
    completeBtn->setFixedSize(120, 40);
    operationLayout->addWidget(completeBtn, 0, Qt::AlignCenter);

    QPushButton* backBtn = createMenuButton("返回");
    backBtn->setFixedSize(120, 40);
    operationLayout->addWidget(backBtn, 0, Qt::AlignCenter);

    // 将操作面板添加到布局
    gameOperationLayout->addWidget(operationPanel, 1); // 操作面板占1份空间

    // 将游戏和操作布局添加到内容布局
    contentLayout->addLayout(gameOperationLayout);

    // 创建内容容器
    QWidget* contentWidget = new QWidget();
    contentWidget->setLayout(contentLayout);

    // 将内容添加到背景
    QVBoxLayout* bgLayout = new QVBoxLayout(bg);
    bgLayout->addWidget(contentWidget);

    // 连接游戏操作信号
    connect(game, &MedicineGame::designOperationAdded, [operationList](const QString& operation) {
        operationList->addItem(operation);
    });

    // 连接返回按钮
    connect(backBtn, &QPushButton::clicked, [this, designPage]() {
        m_stackedWidget->setCurrentWidget(m_centralWidget);
        designPage->deleteLater();
    });

    // 连接完成按钮
    connect(completeBtn, &QPushButton::clicked, [this, game, levelData, filePath, designPage]() {
        // 获取更新后的关卡数据
        QJsonObject updatedLevelData = game->finishDesign(levelData);

        if (updatedLevelData.isEmpty()) {
            QMessageBox::warning(this, "警告", "请至少进行一次操作以打乱药柜！");
            return;
        }

        // 保存更新后的关卡数据
        QJsonDocument saveDoc(updatedLevelData);
        QFile saveFile(filePath);

        if (saveFile.open(QIODevice::WriteOnly)) {
            saveFile.write(saveDoc.toJson());
            saveFile.close();

            QMessageBox::information(this, "成功", "自定义关卡已保存！");

            // 切换回主菜单
            m_stackedWidget->setCurrentWidget(m_centralWidget);
            MusicManager::instance()->switchMusic("taqing.mp3");
            designPage->deleteLater();

        } else {
            QMessageBox::critical(this, "错误", "无法保存关卡文件！");
        }
    });

    // 切换到设计页面
    m_stackedWidget->setCurrentWidget(designPage);
}

void MainWindow::onSettingsClicked()
{
    Setting::instance()->show();
}

void MainWindow::onExitClicked()
{
    QApplication::quit();
}
