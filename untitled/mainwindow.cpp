#include "mainwindow.h"
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>
#include "qapplication.h"

MainWindow* MainWindow::m_instance = nullptr;

MainWindow* MainWindow::instance()
{
    if (!m_instance) {
        m_instance = new MainWindow();
    }
    return m_instance;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_titleWidget(nullptr), m_animationCompleted(false)
{
    qDebug() << "MainWindow构造函数开始";
    setupUI();
    qDebug() << "setupUI完成";
    // 延迟显示存档选择对话框，让黑屏先显示
    QTimer::singleShot(500, this, &MainWindow::showSaveSelectDialog);
    qDebug() << "MainWindow构造函数结束";
}
// 在mainwindow.cpp中修改以下函数：

void MainWindow::setupInitialState()
{
    qDebug() << "setupInitialState开始";

            // 获取实际的窗口大小而不是背景widget的geometry
    QRect targetRect = m_centralWidget->rect();
    if (targetRect.isEmpty()) {
        // 如果centralWidget还没有大小，使用窗口大小
        targetRect = QRect(0, 0, this->width(), this->height());
    }

    qDebug() << "目标大小:" << targetRect;

            // 设置背景widget初始大小（150%）
    int extraWidth = targetRect.width() * 0.5;
    int extraHeight = targetRect.height() * 0.5;
    QRect enlargedRect = targetRect;
    enlargedRect.adjust(-extraWidth/2, -extraHeight/2, extraWidth/2, extraHeight/2);
    m_backgroundWidget->setGeometry(enlargedRect);
    qDebug() << "背景放大后大小:" << enlargedRect;

            // 重新调整标题位置以适应放大的背景
    if (m_titleWidget) {
        int windowWidth = this->width();
        int windowHeight = this->height();
        int titleSize = m_titleWidget->width();

                // 因为背景放大了1.5倍，需要相应调整标题位置
        int x = (enlargedRect.width() - titleSize) / 2;
        int topAreaHeight = enlargedRect.height() / 3;
        int y = (topAreaHeight - titleSize) / 2;
        int initialY = y + 100;

        m_titleWidget->move(x, initialY);
        m_titleWidget->setProperty("finalY", y);

        // 同时更新finalX，确保动画终点位置正确
        m_titleWidget->setProperty("finalX", (windowWidth - titleSize) / 2);

        qDebug() << "调整后的标题位置: x=" << x << ", initialY=" << initialY << ", finalY=" << y;
    }

            // 为标题设置透明度效果
    m_titleOpacity = new QGraphicsOpacityEffect();
    m_titleOpacity->setOpacity(0.0);
    m_titleWidget->setGraphicsEffect(m_titleOpacity);

            // 确保标题在最上层
    m_titleWidget->raise();

            // 为所有按钮设置统一的透明度效果
    QList<QPushButton*> buttons = {
        m_storyModeBtn, m_challengeModeBtn, m_customModeBtn,
        m_gameRulesBtn, m_settingsBtn, m_exitBtn
    };

    m_buttonOpacities.clear();
    for (int i = 0; i < buttons.size(); ++i) {
        QPushButton* btn = buttons[i];
        QGraphicsOpacityEffect* opacity = new QGraphicsOpacityEffect();
        opacity->setOpacity(0.0);
        btn->setGraphicsEffect(opacity);
        m_buttonOpacities.append(opacity);
        qDebug() << "按钮" << i << "透明度设置完成";
    }
}

void MainWindow::showMainWindowAnimation()
{
    qDebug() << "showMainWindowAnimation开始";

            // 切换到主窗口内容
    setCentralWidget(m_stackedWidget);
    m_stackedWidget->setCurrentWidget(m_centralWidget);
    qDebug() << "切换到主窗口内容完成";

            // 创建标题widget如果不存在
    if (!m_titleWidget) {
        createTitleWidget();
    }

            // 使用QTimer延迟设置初始状态，确保布局完成
    QTimer::singleShot(100, this, [this]() {
        // 设置初始状态：背景放大，标题下移，按钮不可见
        setupInitialState();

        // 开始主窗口动画序列
        startMainWindowAnimations();
    });
}

void MainWindow::startMainWindowAnimations()
{
    qDebug() << "startMainWindowAnimations开始";

            // 获取背景widget的当前和目标几何信息
    QRect currentRect = m_backgroundWidget->geometry();
    QRect targetRect = m_centralWidget->rect();

    // 确保targetRect有效
    if (targetRect.isEmpty()) {
        targetRect = QRect(0, 0, this->width(), this->height());
    }

    qDebug() << "背景动画: 从" << currentRect << "到" << targetRect;

            // 创建背景缩放动画
    QPropertyAnimation* bgAnimation = new QPropertyAnimation(m_backgroundWidget, "geometry");
    bgAnimation->setDuration(2000);
    bgAnimation->setStartValue(currentRect);
    bgAnimation->setEndValue(targetRect);
    bgAnimation->setEasingCurve(QEasingCurve::OutQuart);

            // 创建标题上浮动画
    QPropertyAnimation* titleMoveAnimation = new QPropertyAnimation(m_titleWidget, "geometry");
    titleMoveAnimation->setDuration(1500);

            // 获取当前位置和最终位置
    QRect currentTitleRect = m_titleWidget->geometry();
    int finalX = m_titleWidget->property("finalX").toInt();
    int finalY = m_titleWidget->property("finalY").toInt();
    QRect finalTitleRect(finalX, finalY, currentTitleRect.width(), currentTitleRect.height());

    titleMoveAnimation->setStartValue(currentTitleRect);
    titleMoveAnimation->setEndValue(finalTitleRect);
    titleMoveAnimation->setEasingCurve(QEasingCurve::OutQuart);

    qDebug() << "标题上浮动画设置:";
    qDebug() << "  起始位置:" << currentTitleRect;
    qDebug() << "  最终位置:" << finalTitleRect;
    qDebug() << "  上移距离:" << (currentTitleRect.top() - finalTitleRect.top()) << "像素";

            // 创建标题淡入动画
    QPropertyAnimation* titleFadeAnimation = new QPropertyAnimation(m_titleOpacity, "opacity");
    titleFadeAnimation->setDuration(1500);
    titleFadeAnimation->setStartValue(0.0);
    titleFadeAnimation->setEndValue(1.0);
    titleFadeAnimation->setEasingCurve(QEasingCurve::InOutQuad);

            // 创建所有按钮同时淡入的动画组
    QParallelAnimationGroup* buttonsGroup = new QParallelAnimationGroup();

    for (int i = 0; i < m_buttonOpacities.size(); ++i) {
        QPropertyAnimation* btnFadeAnimation = new QPropertyAnimation(m_buttonOpacities[i], "opacity");
        btnFadeAnimation->setDuration(1000);
        btnFadeAnimation->setStartValue(0.0);
        btnFadeAnimation->setEndValue(1.0);
        btnFadeAnimation->setEasingCurve(QEasingCurve::OutQuad);

        buttonsGroup->addAnimation(btnFadeAnimation);
    }

            // 创建完整动画序列
    QSequentialAnimationGroup* completeSequence = new QSequentialAnimationGroup();

            // 第一阶段：背景缩放 + 标题上浮和淡入
    QParallelAnimationGroup* firstPhase = new QParallelAnimationGroup();
    firstPhase->addAnimation(bgAnimation);
    firstPhase->addAnimation(titleMoveAnimation);
    firstPhase->addAnimation(titleFadeAnimation);

            // 第二阶段：所有按钮同时淡入
    completeSequence->addAnimation(firstPhase);
    completeSequence->addPause(300);
    completeSequence->addAnimation(buttonsGroup);

            // 动画完成标记
    connect(completeSequence, &QSequentialAnimationGroup::finished, [this]() {
        m_animationCompleted = true;
        qDebug() << "启动动画序列完成";
        qDebug() << "标题最终位置:" << m_titleWidget->geometry();

        // 确保背景widget填满整个区域
        m_backgroundWidget->setGeometry(m_centralWidget->rect());
    });

    qDebug() << "开始主窗口动画序列";
    completeSequence->start(QAbstractAnimation::DeleteWhenStopped);
}
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

            // 更新背景大小
    if (m_backgroundWidget) {
        m_backgroundWidget->setGeometry(0, 0, width(), height());
    }

            // 更新标题大小和位置
    if (m_titleWidget && m_animationCompleted) {  // 只在动画完成后更新
        int windowWidth = this->width();
        int windowHeight = this->height();

        // 重新计算标题大小（窗口高度的1/2）
        int titleSize = windowHeight / 2;

        // 重新加载并缩放图片
        QPixmap titlePixmap(":/images/title.png");
        if (!titlePixmap.isNull()) {
            titlePixmap = titlePixmap.scaled(titleSize, titleSize,
                                             Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation);
            m_titleWidget->setPixmap(titlePixmap);
        }

        // 更新widget大小
        m_titleWidget->resize(titleSize, titleSize);

        // 重新计算标题位置
        int x = (windowWidth - titleSize) / 2;
        int topAreaHeight = windowHeight / 2;
        int y = (topAreaHeight - titleSize) / 2;

        m_titleWidget->move(x, y);
    }
}

void MainWindow::setupUI()
{
    qDebug() << "setupUI开始";

            // 设置窗口属性
    setWindowTitle("本草华章：我是小药童");
    resize(1280, 720);

            // 创建QStackedWidget作为中央窗口部件
    m_stackedWidget = new QStackedWidget(this);

            // 创建主页面
    m_centralWidget = new QWidget();

            // 创建并设置背景
    m_backgroundWidget = new BackgroundWidget(m_centralWidget);
    m_backgroundWidget->setBackground("bg_main");

            // 使用QVBoxLayout填充整个主页面
    QVBoxLayout* centralLayout = new QVBoxLayout(m_centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->addWidget(m_backgroundWidget);

            // 修改：使用布局而不是固定坐标
    QVBoxLayout* backgroundLayout = new QVBoxLayout(m_backgroundWidget);
    backgroundLayout->setContentsMargins(0, 0, 0, 0);

    // 添加顶部占位空间（1/3窗口高度）
    backgroundLayout->addStretch(1);

    // 创建按钮容器
    QWidget* buttonContainer = new QWidget();

    // 创建按钮布局，使用网格布局
    QGridLayout* buttonLayout = new QGridLayout(buttonContainer);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setVerticalSpacing(20);
    buttonLayout->setHorizontalSpacing(40);
    buttonLayout->setContentsMargins(100, 20, 100, 20);

            // 创建按钮
    m_storyModeBtn = createMenuButton("剧情模式");
    m_challengeModeBtn = createMenuButton("挑战模式");
    m_customModeBtn = createMenuButton("自定义模式");
    m_gameRulesBtn = createMenuButton("游戏规则");
    m_settingsBtn = createMenuButton("设置");
    m_exitBtn = createMenuButton("退出");

            // 使用网格布局排列按钮（2列3行）
    buttonLayout->addWidget(m_storyModeBtn, 0, 0);
    buttonLayout->addWidget(m_challengeModeBtn, 0, 1);
    buttonLayout->addWidget(m_customModeBtn, 1, 0);
    buttonLayout->addWidget(m_gameRulesBtn, 1, 1);
    buttonLayout->addWidget(m_settingsBtn, 2, 0);
    buttonLayout->addWidget(m_exitBtn, 2, 1);

            // 将按钮容器添加到背景布局（占2/3空间）
    backgroundLayout->addWidget(buttonContainer, 2);

            // 将页面添加到QStackedWidget
    m_stackedWidget->addWidget(m_centralWidget);

            // 连接信号和槽
    connect(m_storyModeBtn, &QPushButton::clicked, this, &MainWindow::onStoryModeClicked);
    connect(m_challengeModeBtn, &QPushButton::clicked, this, &MainWindow::onChallengeModeClicked);
    connect(m_customModeBtn, &QPushButton::clicked, this, &MainWindow::onCustomModeClicked);
    connect(m_gameRulesBtn, &QPushButton::clicked, this, &MainWindow::onGameRulesClicked);
    connect(m_settingsBtn, &QPushButton::clicked, this, &MainWindow::onSettingsClicked);
    connect(m_exitBtn, &QPushButton::clicked, this, &MainWindow::onExitClicked);

    qDebug() << "setupUI完成，按钮数量:" << 6;
}


void MainWindow::createTitleWidget()
{
    qDebug() << "createTitleWidget开始";

            // 获取当前窗口大小（考虑全屏情况）
    int windowWidth = this->width();
    int windowHeight = this->height();

            // 修改：让标题直接作为主窗口的子控件
    m_titleWidget = new QLabel(this);  // 改为 this 而不是 m_backgroundWidget
    QPixmap titlePixmap(":/images/title.png");
            // 标题大小：取窗口高度的1/3作为正方形边长
    int titleSize = windowHeight / 2;

    if (!titlePixmap.isNull()) {
        qDebug() << "标题图片加载成功";
        titlePixmap = titlePixmap.scaled(titleSize, titleSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_titleWidget->setPixmap(titlePixmap);
    } else {
        qDebug() << "标题图片加载失败，使用文字";
        m_titleWidget->setText("本草华章：我是小药童");
        m_titleWidget->setStyleSheet("color: #8B4513; font-size: 48px; font-weight: bold; background: transparent;");
    }

    m_titleWidget->setAlignment(Qt::AlignCenter);
    m_titleWidget->resize(titleSize, titleSize);

            // 由于背景widget使用了布局，我们需要将标题作为浮动widget
            // 设置为背景widget的子widget，但不加入布局
    m_titleWidget->setParent(m_backgroundWidget);

    // 计算位置：水平居中，垂直在上1/3区域居中
    int x = (windowWidth - titleSize) / 2;
    int topAreaHeight = windowHeight / 2;
    int y = (topAreaHeight - titleSize) / 2;

            // 设置初始位置（稍微下移用于动画）
    int initialY = y + 100;
    m_titleWidget->move(x, initialY);

            // 保存最终位置
    m_titleWidget->setProperty("finalX", x);
    m_titleWidget->setProperty("finalY", y);
    m_titleWidget->raise();
    m_titleWidget->show();

    qDebug() << "标题widget创建完成";
}

QPushButton* MainWindow::createMenuButton(const QString& text)
{
    QPushButton* button = new QPushButton(text, this);
    button->setMinimumSize(240, 70); // 增大按钮尺寸
    button->setFont(QFont("Microsoft YaHei", 16, QFont::Bold)); // 增大字体

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

void MainWindow::showLogoAnimation()
{
    qDebug() << "showLogoAnimation开始";

    // 创建logo显示widget，占满整个窗口
    m_logoWidget = new QWidget(this);
    m_logoWidget->setFixedSize(this->size());
    m_logoWidget->setStyleSheet("background-color: black;");
    qDebug() << "logo widget创建完成，大小:" << m_logoWidget->size();

    // 创建logo标签
    QLabel* logoLabel = new QLabel(m_logoWidget);
    logoLabel->setAlignment(Qt::AlignCenter);

    QPixmap logoPixmap(":/images/logo.png");
    if (!logoPixmap.isNull()) {
        qDebug() << "logo图片加载成功，原始大小:" << logoPixmap.size();

        // 高度占满窗口，宽度按比例缩放
        int windowHeight = m_logoWidget->height();
        // 计算按比例缩放后的宽度
        double aspectRatio = (double)logoPixmap.width() / logoPixmap.height();
        int scaledWidth = (int)(windowHeight * aspectRatio);

        QPixmap scaledPixmap = logoPixmap.scaled(scaledWidth, windowHeight,
                                                 Qt::IgnoreAspectRatio,
                                                 Qt::SmoothTransformation);
        logoLabel->setPixmap(scaledPixmap);
        // 调整label大小以适应缩放后的图片
        logoLabel->resize(scaledPixmap.size());
        // 居中显示
        logoLabel->move((m_logoWidget->width() - scaledPixmap.width()) / 2, 0);

        qDebug() << "Logo缩放后大小:" << scaledPixmap.size();
        qDebug() << "Logo位置:" << logoLabel->geometry();
    } else {
        qDebug() << "logo图片加载失败，使用文字替代";
        logoLabel->setText("本草华章");
        logoLabel->setStyleSheet("color: white; font-size: 72px; font-weight: bold; background-color: black;");
        logoLabel->resize(m_logoWidget->size());
    }

    // 设置透明度效果
    QGraphicsOpacityEffect* logoOpacity = new QGraphicsOpacityEffect();
    logoOpacity->setOpacity(0.0);
    logoLabel->setGraphicsEffect(logoOpacity);
    qDebug() << "logo透明度效果设置完成";

    setCentralWidget(m_logoWidget);
    qDebug() << "设置logo widget为中央部件";

    // 强制刷新界面
    m_logoWidget->show();
    logoLabel->show();
    QApplication::processEvents();

    // 创建logo淡入动画
    QPropertyAnimation* logoFadeIn = new QPropertyAnimation(logoOpacity, "opacity");
    logoFadeIn->setDuration(2000);
    logoFadeIn->setStartValue(0.0);
    logoFadeIn->setEndValue(1.0);
    logoFadeIn->setEasingCurve(QEasingCurve::InOutQuad);

    // logo停留1秒后淡出
    QPropertyAnimation* logoFadeOut = new QPropertyAnimation(logoOpacity, "opacity");
    logoFadeOut->setDuration(1500);
    logoFadeOut->setStartValue(1.0);
    logoFadeOut->setEndValue(0.0);
    logoFadeOut->setEasingCurve(QEasingCurve::InOutQuad);

    // 创建完整的logo动画序列
    QSequentialAnimationGroup* logoSequence = new QSequentialAnimationGroup();
    logoSequence->addAnimation(logoFadeIn);
    logoSequence->addPause(1000); // 停留1秒
    logoSequence->addAnimation(logoFadeOut);

    // logo动画完成后显示主窗口
    connect(logoSequence, &QSequentialAnimationGroup::finished, [this]() {
        qDebug() << "Logo动画序列完成";
        showMainWindowAnimation();
    });

    qDebug() << "开始logo动画序列";
    logoSequence->start(QAbstractAnimation::DeleteWhenStopped);
}


void MainWindow::showBlackScreen()
{
    qDebug() << "showBlackScreen开始";
    // 创建黑屏widget
    m_blackScreenWidget = new QWidget(this);
    m_blackScreenWidget->setStyleSheet("background-color: black;");
    setCentralWidget(m_blackScreenWidget);
    qDebug() << "showBlackScreen完成，黑屏widget大小:" << m_blackScreenWidget->size();
}

void MainWindow::showSaveSelectDialog()
{
    qDebug() << "showSaveSelectDialog开始";
    SaveSelectDialog* dialog = new SaveSelectDialog(this);
    if (dialog->exec() == QDialog::Accepted) {
        m_saveManager = SaveManager::instance();
        QString playerName = m_saveManager->getPlayerName();
        setWindowTitle(QString("本草华章：我是小药童 - %1").arg(playerName));
        qDebug() << "存档选择完成，玩家名称:" << playerName;

        // 存档选择完成后开始启动动画
        startIntroAnimation();
    } else {
        QMessageBox::critical(this, "错误", "必须选择或创建存档才能开始游戏！");
        QApplication::quit();
    }
    dialog->deleteLater();
}

void MainWindow::startIntroAnimation()
{
    qDebug() << "startIntroAnimation开始";
    // 第一阶段：显示logo
    showLogoAnimation();
}


MainWindow::~MainWindow()
{
    m_instance = nullptr;
}

void MainWindow::cleanupGamePages()
{
    // 获取当前页面
    QWidget* currentWidget = m_stackedWidget->currentWidget();

    // 遍历所有页面，删除除了主页面以外的所有页面
    for (int i = m_stackedWidget->count() - 1; i >= 0; --i) {
        QWidget* widget = m_stackedWidget->widget(i);
        if (widget != m_centralWidget && widget != currentWidget) {
            m_stackedWidget->removeWidget(widget);
            widget->deleteLater();
        }
    }
}

void MainWindow::switchToMainPage()
{
    // 先切换到主页面
    m_stackedWidget->setCurrentWidget(m_centralWidget);

    // 延迟清理其他页面，避免正在切换时删除
    QTimer::singleShot(100, this, &MainWindow::cleanupGamePages);
    MusicManager::instance()->setMusic("taqing.mp3");
    MusicManager::instance()->playMusic();
}


// 添加游戏规则按钮的点击处理函数
void MainWindow::onGameRulesClicked()
{
    // 创建自定义对话框
    QDialog* rulesDialog = new QDialog(this);
    rulesDialog->setWindowTitle("游戏规则");
    rulesDialog->setFixedSize(800, 600);
    rulesDialog->setStyleSheet("QDialog { border-image: url(:/images/next_chapter.png) stretch; }");

    QVBoxLayout* dialogLayout = new QVBoxLayout(rulesDialog);

            // 创建游戏规则文本
    QLabel* rulesLabel = new QLabel(rulesDialog);
    rulesLabel->setText(
        "<h2>药材柜游戏规则</h2>"
        "<p>游戏背景：在方形的中药柜上有大量抽屉，每个抽屉放着两种药材。"
        "你需要打开正确的抽屉，取出符合药材清单的药材。</p>"
        "<h3>游戏规则：</h3>"
        "<ol>"
        "<li>每个抽屉上标有两种药材名称，抽屉可能处于打开或关闭状态。</li>"
        "<li>点击抽屉上的某个药材名称，所有标有这个药材名的抽屉都会翻转开合状态（打开变关闭，关闭变打开）。</li>"
        "<li>左侧的药材清单显示游戏目标—每种药材需要的份数。</li>"
        "<li>你的目标是：打开的抽屉中的所有药材加起来，恰好符合药材清单上的要求（种类和数量都完全一致）。</li>"
        "<li>只有通过正确的操作顺序，才能让药柜的开合状态符合药材清单的要求。</li>"
        "<li>游戏分数规则：初始分数为(2000+行*列*100+药材种类*500)，每移动1步减少200分，自进入游戏开始，每过去1秒扣10分。</li>"
        "</ol>"
        "<p>游戏提示：如果遇到困难，可以点击“窥天镜”(答案)按钮获取提示，或者使用“万能药材”忽视一种药物。</p>"
        );

    rulesLabel->setWordWrap(true);
    rulesLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    rulesLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-size: 16px;"
        "   background-color: rgba(0, 0, 0, 180);"
        "   padding: 20px;"
        "   border-radius: 10px;"
        "}"
        );

            // 添加确定按钮
    QPushButton* okButton = new QPushButton("我知道了", rulesDialog);
    okButton->setMinimumSize(120, 40);
    okButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 255, 255, 200);"
        "   border: 2px solid #8B4513;"
        "   border-radius: 8px;"
        "   color: #8B4513;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(139, 69, 19, 200);"
        "   color: white;"
        "}"
        );

            // 创建布局
    dialogLayout->addWidget(rulesLabel);
    dialogLayout->addWidget(okButton, 0, Qt::AlignCenter);
    dialogLayout->setContentsMargins(30, 30, 30, 30);

            // 连接按钮点击事件
    connect(okButton, &QPushButton::clicked, rulesDialog, &QDialog::accept);

            // 显示对话框
    rulesDialog->exec();
    rulesDialog->deleteLater();
}


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

            // 连接游戏前故事完成信号（开始游戏）
    connect(storyMode, &StoryMode::storyCompleted, this, [this, storyPage, storyMode, chapterNumber](QJsonObject gameParams) {
        // 对于第一章进行特殊处理
        if (chapterNumber == 1) {
            // 第一章不显示游戏，直接调用游戏完成逻辑
            QTimer::singleShot(100, [storyMode]() {
                if (storyMode) {
                    storyMode->onGameCompleted();
                }
            });
            return;
        }

                // 创建游戏页面
        QWidget* gamePage = new QWidget();
        m_stackedWidget->addWidget(gamePage);

                // 获取游戏参数
        int medicineTypes = gameParams["medicineTypes"].toInt(5);
        int rows = gameParams["rows"].toInt(3);
        int cols = gameParams["cols"].toInt(3);
        int operationCount = gameParams["operationCount"].toInt(3);

        qDebug() << "游戏参数:" << medicineTypes << rows << cols << operationCount;

                // 创建布局
        QVBoxLayout* gameLayout = new QVBoxLayout(gamePage);
        gameLayout->setContentsMargins(0, 0, 0, 0);

                // 修复：正确创建游戏实例，传递 design=false 参数
        MedicineGame* game = new MedicineGame(gamePage, false);

                // 添加到布局
        gameLayout->addWidget(game);

                // 先切换到游戏页面，再初始化游戏
        m_stackedWidget->setCurrentWidget(gamePage);

                // 使用 QTimer::singleShot 延迟初始化，确保界面完全显示后再初始化游戏
        QTimer::singleShot(100, [game, medicineTypes, rows, cols, operationCount]() {
            if (game) {
                game->initGame(medicineTypes, rows, cols, operationCount);
            }
        });

                // 设置游戏完成时的回调
        connect(game, &MedicineGame::gameCompleted, this, [this, gamePage, storyPage, storyMode]() {
            qDebug() << "游戏完成，返回故事模式";

                    // 游戏完成后，删除游戏页面并返回故事模式
            m_stackedWidget->removeWidget(gamePage);
            gamePage->deleteLater();

                    // 切换回故事模式页面
            m_stackedWidget->setCurrentWidget(storyPage);

                    // 使用 QTimer::singleShot 延迟调用，确保界面切换完成
            QTimer::singleShot(100, [storyMode]() {
                if (storyMode) {
                    storyMode->onGameCompleted();
                }
            });
        });
    });

            // 连接整个章节完成信号（游戏后对话也完成）
    connect(storyMode, &StoryMode::chapterCompleted, this, [this, storyPage, chapterNumber, saveManager]() {
        qDebug() << "章节完成";

                // 保存进度
        saveManager->setStoryProgress(chapterNumber);
        saveManager->saveSaveFile(saveManager->getCurrentSaveFile());
        qDebug() << "已保存游戏进度：章节" << chapterNumber;

                // 创建自定义对话框
        QDialog* nextChapterDialog = new QDialog(this);
        nextChapterDialog->setWindowTitle("章节完成");
        nextChapterDialog->setFixedSize(400, 300);
        nextChapterDialog->setStyleSheet("QDialog { border-image: url(:/images/next_chapter.png) stretch; }");

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
        connect(returnButton, &QPushButton::clicked, [this, nextChapterDialog, storyPage, chapterNumber]() {
            nextChapterDialog->accept();

                    // 显示进度已保存的提示
            QMessageBox::information(this, "游戏进度", QString("已保存至第%1章").arg(chapterNumber));

                    // 先移除页面
            m_stackedWidget->removeWidget(storyPage);

            // 切换回主菜单
            m_stackedWidget->setCurrentWidget(m_centralWidget);

                    // 恢复主菜单音乐
            MusicManager::instance()->switchMusic("taqing.mp3");

                    // 删除故事页面
            storyPage->deleteLater();

            // 清理可能存在的其他游戏页面
            QTimer::singleShot(100, this, &MainWindow::cleanupGamePages);
        });


                // 继续下一章按钮的点击事件
        connect(nextButton, &QPushButton::clicked, [this, nextChapterDialog, storyPage, chapterNumber]() {
            nextChapterDialog->accept();

                    // 删除当前故事页面
            m_stackedWidget->removeWidget(storyPage);
            storyPage->deleteLater();

                    // 加载下一章节
            this->loadChapter(chapterNumber + 1);
        });

                // 显示对话框
        nextChapterDialog->exec();
        nextChapterDialog->deleteLater();
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

            // 获取已解锁的关卡
    int unlockedLevel = m_saveManager->getchallengeUnlockedLevel();

            // 创建关卡选择网格
    QGridLayout* levelGrid = new QGridLayout();
    levelGrid->setSpacing(20);

            // 创建9个关卡按钮
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            int level = row * 3 + col + 1;

                    // 创建一个容器来放置按钮和分数标签
            QWidget* levelContainer = new QWidget();
            QVBoxLayout* containerLayout = new QVBoxLayout(levelContainer);
            containerLayout->setSpacing(5);

            QPushButton* levelButton = new QPushButton();
            levelButton->setMinimumSize(100, 100);
            levelButton->setFont(QFont("Microsoft YaHei", 18, QFont::Bold));

                    // 根据解锁状态设置按钮内容和样式
            if (level <= unlockedLevel) {
                // 已解锁的关卡显示数字
                levelButton->setText(QString::number(level));
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

                        // 显示最高分
                int highScore = m_saveManager->getchallengeHighScore(level);
                QLabel* scoreLabel = new QLabel(QString("最高分: %1").arg(highScore));
                scoreLabel->setAlignment(Qt::AlignCenter);
                scoreLabel->setStyleSheet("color: #8B4513; font-weight: bold; font-size: 14px;");
                containerLayout->addWidget(scoreLabel);

                        // 连接按钮信号到启动关卡的槽
                connect(levelButton, &QPushButton::clicked, this, [this, level]() {
                    startChallengeLevel(level);
                });
            } else {
                // 未解锁的关卡显示锁图标
                levelButton->setIcon(QIcon(":/images/lock.png"));
                levelButton->setIconSize(QSize(60, 60));
                levelButton->setStyleSheet(
                    "QPushButton {"
                    "    background-color: rgba(100, 100, 100, 180);"
                    "    border: 3px solid #555555;"
                    "    border-radius: 15px;"
                    "}"
                    );

                        // 未解锁提示标签
                QLabel* lockedLabel = new QLabel("未解锁");
                lockedLabel->setAlignment(Qt::AlignCenter);
                lockedLabel->setStyleSheet("color: #555555; font-weight: bold; font-size: 14px;");
                containerLayout->addWidget(lockedLabel);
            }

            containerLayout->addWidget(levelButton);
            levelGrid->addWidget(levelContainer, row, col);
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

// 修改 startChallengeLevel 函数
void MainWindow::startChallengeLevel(int level)
{
    // 根据难度级别计算游戏参数
    int medicineTypes = 5 + level;         // 药材种类数随难度增加
    int rows = 2 + (level - 1) / 3;        // 行数：难度1-3为2行，4-6为3行，7-9为4行
    int cols = 2 + (level - 1) % 3;        // 列数：难度1,4,7为2列，2,5,8为3列，3,6,9为4列
    int operationCount = level * 1.5;      // 随机操作次数随难度增加

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
    connect(game, &MedicineGame::gameCompleted, this, [this, gamePage, level, game]() {
        // 获取游戏分数并更新存档
        int score = game->getScore();
        int currentHighScore = m_saveManager->getchallengeHighScore(level);

                // 如果新分数更高，则更新存档
        if (score > currentHighScore) {
            m_saveManager->setchallengeHighScore(level, score);
            m_saveManager->saveSaveFile(m_saveManager->getCurrentSaveFile());

                    // 显示新高分提示
            QMessageBox::information(this, "新高分!",
                                     QString("恭喜你获得了新的高分: %1分!").arg(score));
        }

                // 延迟一段时间后返回选关界面
        QTimer::singleShot(2000, this, [this, gamePage]() {
            // 删除现有游戏页面
            m_stackedWidget->removeWidget(gamePage);
            gamePage->deleteLater();

                    // 删除旧的选关界面
            QWidget* oldLevelSelectWidget = m_stackedWidget->widget(m_stackedWidget->count() - 1);
            m_stackedWidget->removeWidget(oldLevelSelectWidget);
            oldLevelSelectWidget->deleteLater();

                    // 重新创建选关界面（重新调用onChallengeModeClicked）
            onChallengeModeClicked();
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

            // 创建标题
    QLabel* titleLabel = new QLabel("自定义药柜设置");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #8B4513; background-color: rgba(255, 245, 225, 0.8); padding: 10px; border-radius: 10px;");

            // 创建内容布局
    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(20);

            // 左侧药材列表
    QGroupBox* medicineGroup = new QGroupBox("可用药材");
    medicineGroup->setStyleSheet("QGroupBox { background-color: rgba(255, 245, 225, 0.8); border: 2px solid #8B4513; border-radius: 10px; font-weight: bold; color: #8B4513; padding: 10px; } "
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top center; padding: 0 10px; }");

    QVBoxLayout* medicineLayout = new QVBoxLayout(medicineGroup);
    medicineLayout->setContentsMargins(15, 25, 15, 15);

    QListWidget* medicineListWidget = new QListWidget();
    medicineListWidget->setStyleSheet("QListWidget { background-color: rgba(255, 250, 240, 0.9); border: 1px solid #CD853F; border-radius: 5px; }"
        "QListWidget::item { padding: 6px; font-size: 14px; color: #654321; }"
        "QListWidget::item:selected { background-color: #DEB887; color: white; }");

    for (const QString& med : medicines) {
        new QListWidgetItem(med, medicineListWidget);
    }
    medicineLayout->addWidget(medicineListWidget);

            // 右侧药柜设置
    QGroupBox* drawerGroup = new QGroupBox("药柜设置");
    drawerGroup->setStyleSheet("QGroupBox { background-color: rgba(255, 245, 225, 0.8); border: 2px solid #8B4513; border-radius: 10px; font-weight: bold; color: #8B4513; padding: 10px; } "
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top center; padding: 0 10px; }");

    QVBoxLayout* drawerLayoutContainer = new QVBoxLayout(drawerGroup);
    drawerLayoutContainer->setContentsMargins(15, 25, 15, 15);

    QGridLayout* drawersGrid = new QGridLayout();
    drawersGrid->setSpacing(15);

            // 储存药柜和抽屉状态的数据结构
    QVector<QVector<QPushButton*>> drawerButtons;
    QVector<QVector<QComboBox*>> leftComboBoxes;
    QVector<QVector<QComboBox*>> rightComboBoxes;
    QVector<QVector<bool>> drawerStates;

            // 创建统一样式的下拉框样式
    QString comboStyle = "QComboBox { background-color: rgba(255, 250, 240, 0.9); border: 1px solid #CD853F; border-radius: 5px; padding: 3px; color: #654321; }"
        "QComboBox::drop-down { border: 0px; }"
        "QComboBox QAbstractItemView { background-color: #FFF8DC; selection-background-color: #DEB887; selection-color: white; border: 1px solid #CD853F; }";

            // 创建药柜网格
    for (int r = 0; r < rows; r++) {
        QVector<QPushButton*> rowButtons;
        QVector<QComboBox*> rowLeftCombos;
        QVector<QComboBox*> rowRightCombos;
        QVector<bool> rowStates;

        for (int c = 0; c < cols; c++) {
            // 创建药柜容器
            QWidget* drawerWidget = new QWidget();
            QVBoxLayout* drawerLayout = new QVBoxLayout(drawerWidget);
            drawerLayout->setContentsMargins(5, 5, 5, 5);
            drawerLayout->setSpacing(5);

                    // 药柜下拉框和按钮水平布局
            QHBoxLayout* selectionLayout = new QHBoxLayout();

                    // 左侧药材选择
            QComboBox* leftCombo = new QComboBox();
            leftCombo->addItems(medicines);
            leftCombo->setStyleSheet(comboStyle);
            leftCombo->setFixedWidth(80);

                    // 右侧药材选择
            QComboBox* rightCombo = new QComboBox();
            rightCombo->addItems(medicines);
            rightCombo->setStyleSheet(comboStyle);
            rightCombo->setFixedWidth(80);
            if (medicines.size() > 1) {
                rightCombo->setCurrentIndex(1); // 默认选择第二种药材
            }

                    // 药柜按钮（显示开/关状态）
            QPushButton* drawerBtn = new QPushButton();
            drawerBtn->setCheckable(true);
            drawerBtn->setFixedSize(160, 80);

                    // 设置抽屉背景图片和样式
            drawerBtn->setStyleSheet(
                "QPushButton { border-image: url(:/images/drawer.png) stretch; border: none; color: white; font-weight: bold; }"
                "QPushButton:checked { border-image: url(:/images/drawer.png) stretch; }"
                "QPushButton:hover { opacity: 0.9; }"
                );

                    // 添加状态标签
            QLabel* stateLabel = new QLabel("关闭");
            stateLabel->setAlignment(Qt::AlignCenter);
            stateLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #8B4513; background-color: rgba(255, 255, 255, 0.7); border-radius: 5px; padding: 3px;");

                    // 连接药柜按钮点击事件
            connect(drawerBtn, &QPushButton::toggled, [stateLabel](bool checked) {
                stateLabel->setText(checked ? "打开" : "关闭");
                stateLabel->setStyleSheet(checked ?
                                              "font-size: 14px; font-weight: bold; color: white; background-color: rgba(50, 150, 50, 0.7); border-radius: 5px; padding: 3px;" :
                                              "font-size: 14px; font-weight: bold; color: #8B4513; background-color: rgba(255, 255, 255, 0.7); border-radius: 5px; padding: 3px;");
            });

                    // 添加组件到布局
            selectionLayout->addWidget(leftCombo);
            selectionLayout->addStretch();
            selectionLayout->addWidget(rightCombo);

            drawerLayout->addLayout(selectionLayout);
            drawerLayout->addWidget(drawerBtn, 0, Qt::AlignCenter);
            drawerLayout->addWidget(stateLabel, 0, Qt::AlignCenter);

                    // 添加阴影效果
            QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();
            shadowEffect->setBlurRadius(15);
            shadowEffect->setColor(QColor(0, 0, 0, 80));
            shadowEffect->setOffset(3, 3);
            drawerWidget->setGraphicsEffect(shadowEffect);

                    // 设置抽屉容器的样式
            drawerWidget->setStyleSheet("background-color: rgba(255, 250, 240, 0.7); border-radius: 10px; padding: 5px;");

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

            // 创建按钮区域
    QWidget* buttonContainer = new QWidget();
    buttonContainer->setStyleSheet("background-color: rgba(255, 250, 240, 0.7); border-radius: 10px; padding: 10px;");

    QHBoxLayout* btnLayout = new QHBoxLayout(buttonContainer);
    btnLayout->setSpacing(20);

            // 创建保存按钮
    QPushButton* saveBtn = createMenuButton("储存关卡");
    saveBtn->setFixedSize(150, 50);
    saveBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: rgba(139, 69, 19, 0.8);"
        "    border: 2px solid #654321;"
        "    border-radius: 10px;"
        "    color: white;"
        "    font-weight: bold;"
        "    font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(160, 82, 45, 0.9);"
        "}"
        "QPushButton:pressed {"
        "    background-color: #654321;"
        "}"
        );

    QPushButton* backBtn = createMenuButton("返回");
    backBtn->setFixedSize(150, 50);
    backBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: rgba(178, 34, 34, 0.8);"
        "    border: 2px solid #8B0000;"
        "    border-radius: 10px;"
        "    color: white;"
        "    font-weight: bold;"
        "    font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(205, 92, 92, 0.9);"
        "}"
        "QPushButton:pressed {"
        "    background-color: #8B0000;"
        "}"
        );

    btnLayout->addWidget(backBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(saveBtn);

    drawerLayoutContainer->addWidget(buttonContainer);

            // 添加组件到内容布局
    contentLayout->addWidget(medicineGroup, 1);
    contentLayout->addWidget(drawerGroup, 3);

            // 创建内容容器
    QWidget* contentWidget = new QWidget();
    QVBoxLayout* containerLayout = new QVBoxLayout(contentWidget);
    containerLayout->setContentsMargins(20, 20, 20, 20);
    containerLayout->setSpacing(20);
    containerLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    containerLayout->addLayout(contentLayout);

            // 将内容添加到背景
    QVBoxLayout* bgLayout = new QVBoxLayout(bg);
    bgLayout->addWidget(contentWidget, 0, Qt::AlignCenter);

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
                QString leftMed = leftComboBoxes[r][c]->currentText();
                QString rightMed = rightComboBoxes[r][c]->currentText();
                medicineList[leftMed] =0;
                medicineList[rightMed] = 0;
            }
        }
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
    connect(completeBtn, &QPushButton::clicked, [this, levelData, operationList, filePath, designPage]() {
        // 获取操作记录
        QJsonArray operations;
        for (int i = 0; i < operationList->count(); i++) {
            operations.append(operationList->item(i)->text());
        }

                // 将操作记录添加到关卡数据
        QJsonObject updatedLevelData = levelData;
        updatedLevelData["operations"] = operations;

                // 保存更新后的关卡文件
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            QJsonDocument doc(updatedLevelData);
            file.write(doc.toJson());
            file.close();

            QMessageBox::information(designPage, "成功", "关卡设计完成！");

                    // 返回主菜单
            m_stackedWidget->setCurrentWidget(m_centralWidget);
            designPage->deleteLater();
        } else {
            QMessageBox::warning(designPage, "错误", "无法保存文件！");
        }
    });

            // 切换到设计页面
    m_stackedWidget->setCurrentWidget(designPage);
}

// 设置按钮点击处理
void MainWindow::onSettingsClicked()
{
    // 使用已经写好的 Setting 类
    Setting* settingsDialog = Setting::instance();
    settingsDialog->exec();
}


// 退出按钮点击处理
void MainWindow::onExitClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认退出",
        "确定要退出游戏吗？",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        QApplication::quit();
    }
}
