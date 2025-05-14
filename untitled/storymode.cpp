#include "storymode.h"
#include "musicmanager.h"
#include "setting.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QMessageBox>
#include <QTimer>
StoryMode::StoryMode(QWidget *parent) : QWidget(parent), m_currentDialogue(0)
{
    setupUI();
}

StoryMode::~StoryMode()
{
}

void StoryMode::setupUI()
{
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

            // 创建背景
    m_backgroundWidget = new BackgroundWidget(this);
    mainLayout->addWidget(m_backgroundWidget);

    // 创建设置按钮
    m_settingsButton = new QPushButton(this);
    m_settingsButton->setIcon(QIcon(":/images/setting.png"));
    m_settingsButton->setIconSize(QSize(80, 80));
    m_settingsButton->setFixedSize(100, 100);
    m_settingsButton->setStyleSheet("QPushButton { background-color: transparent; border: none; }");
    m_settingsButton->setCursor(Qt::PointingHandCursor);

    // 添加章节标题到背景布局
    QVBoxLayout* bgLayout = new QVBoxLayout(m_backgroundWidget);
    bgLayout->setContentsMargins(20, 20, 20, 20);
    bgLayout->addStretch();

    // 创建对话框
    m_dialogueBox = new QWidget(this);
    m_dialogueBox->setObjectName("dialogueBox");
    m_dialogueBox->setStyleSheet(
        "#dialogueBox {"
        "   background-color: rgba(0, 0, 0, 180);"
        "   border: 4px solid #8B4513;"  // 从2px改为3px
        "   border-radius: 20px;"  // 从15px改为20px
        "}"
        );

    QHBoxLayout* dialogueLayout = new QHBoxLayout(m_dialogueBox);
    dialogueLayout->setContentsMargins(15, 15, 15, 15);

    // 头像
    m_avatarLabel = new QLabel();

    dialogueLayout->addWidget(m_avatarLabel);

    // 对话内容区域
    QWidget* textContainer = new QWidget();
    QVBoxLayout* textLayout = new QVBoxLayout(textContainer);
    textLayout->setContentsMargins(10, 5, 10, 5);

    // 角色名称
    m_characterLabel = new QLabel();
    m_characterLabel->setStyleSheet(
        "QLabel {"
        "   color: #FFC125;"
        "   font-size: 24px;"  // 从16px改为24px
        "   font-weight: bold;"
        "}"
        );
    textLayout->addWidget(m_characterLabel);

    // 对话内容
    m_textLabel = new QLabel();
    m_textLabel->setWordWrap(true);
    m_textLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-size: 20px;"  // 从14px改为20px
        "}"
        );
    textLayout->addWidget(m_textLabel);

    dialogueLayout->addWidget(textContainer, 1);

    // 下一步按钮
    m_nextButton = new QPushButton(">>点击任意位置继续");
    m_nextButton->setFixedSize(200, 40);
    m_nextButton->setCursor(Qt::PointingHandCursor);
    m_nextButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(139, 69, 19, 200);"
        "   color: white;"
        "   border: 2px solid #8B4513;"
        "   border-radius: 10px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #8B4513;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #654321;"
        "}"
        );
    dialogueLayout->addWidget(m_nextButton, 0, Qt::AlignBottom | Qt::AlignRight);

    // 将对话框添加到背景布局中
    bgLayout->addWidget(m_dialogueBox, 0, Qt::AlignBottom);

    // 初始隐藏对话框
    m_dialogueBox->hide();

    // 创建章节过渡层
    m_chapterTransition = new ChapterTransition(this);
    m_chapterTransition->resize(size());
    connect(m_chapterTransition, &ChapterTransition::transitionFinished,
            this, &StoryMode::onChapterTransitionFinished);

    // 连接信号和槽
    connect(m_nextButton, &QPushButton::clicked, this, &StoryMode::nextDialogue);
    connect(m_settingsButton, &QPushButton::clicked, this, &StoryMode::onSettingsClicked);
    // 确保按钮不受布局影响
    m_settingsButton->setParent(this);  // 确保直接是窗口的子控件
    m_settingsButton->raise();  // 确保在最上层

}

bool StoryMode::loadChapter(int chapter)
{
    // 重置状态
    m_currentDialogue = 0;
    m_dialogues.clear();

            // 加载章节文件
    if (!loadChapterFile(chapter)) {
        return false;
    }

            // 更新背景
    m_backgroundWidget->setBackground(m_background);

            // 使用新的章节过渡效果
    m_chapterTransition->resize(size()); // 确保尺寸正确
    // 延迟 0.5 秒再开始过渡动画
    QTimer::singleShot(500, this, [this]() {
        m_chapterTransition->showChapter(m_chapterTitle);
    });
    return true;
}
    // 添加章节过渡完成的槽函数
    void StoryMode::onChapterTransitionFinished()
    {
        // 章节过渡效果完成后，显示第一个对话
        showCurrentDialogue();
    }
void StoryMode::resizeEvent(QResizeEvent *event)
    {
        QWidget::resizeEvent(event);

                // 保持 ChapterTransition 大小
        if (m_chapterTransition) {
            m_chapterTransition->resize(size());
        }

                // 重新定位设置按钮
        if (m_settingsButton) {
            m_settingsButton->move(width()-100-20, 20);
        }

                // —— 新增：如果当前对话框和头像都在显示，则重置头像位置 ——
        if (m_dialogueBox->isVisible() && m_avatarLabel->isVisible()) {
            // 强制布局更新，确保 m_dialogueBox 的 geometry 是最新的
            m_dialogueBox->layout()->activate();

                    // 计算可用空间
            int leftMargin      = 20;
            int avatarBottom    = m_dialogueBox->y();
            int topPosition     = avatarBottom - m_avatarLabel->height();
            topPosition         = qMax(10, topPosition);

                    // 限制横向不超出左侧边界
            int maxX = width() - m_avatarLabel->width() - 10;
            int x     = qMin(leftMargin, maxX);

            m_avatarLabel->setParent(this);
            m_avatarLabel->move(x, topPosition);
            m_avatarLabel->raise();
        }
    }
bool StoryMode::loadChapterFile(int chapter)
{
    QString fileName = QString("chapter_%1.json").arg(chapter);
    QByteArray data;
    bool fileLoaded = false;

    // 首先尝试从资源文件加载
    QString qrcPath = QString(":/storymode/%1").arg(fileName);
    QFile qrcFile(qrcPath);

    if (qrcFile.exists() && qrcFile.open(QIODevice::ReadOnly)) {
        qDebug() << "从资源文件加载章节:" << qrcPath;
        data = qrcFile.readAll();
        qrcFile.close();
        fileLoaded = true;
    } else {
        qDebug()<<"加载不出章节啊";
    }

    if (!fileLoaded) {
        return false;
    }

    // 解析JSON数据
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject chapterData = doc.object();

    // 解析章节数据
    m_chapterTitle = chapterData["title"].toString();
    m_background = chapterData["background"].toString();

    // 解析对话数据
    QJsonArray dialoguesArray = chapterData["dialogues"].toArray();
    for (int i = 0; i < dialoguesArray.size(); i++) {
        QJsonObject dialogueObj = dialoguesArray[i].toObject();

        DialogueData dialogue;
        dialogue.character = dialogueObj["character"].toString();
        dialogue.avatar = dialogueObj["avatar"].toString();
        dialogue.text = dialogueObj["text"].toString();
        dialogue.music = dialogueObj["music"].toString();  // 解析音乐字段

        m_dialogues.append(dialogue);
    }

    // 解析游戏参数
    m_gameParams = chapterData["game"].toObject();

    return true;
}

void StoryMode::showCurrentDialogue()
{
    if (m_currentDialogue >= m_dialogues.size()) {
        // 所有对话已完成，准备开始游戏
        emit storyCompleted(m_gameParams);

        return;
    }

            // 获取当前对话
    const DialogueData& dialogue = m_dialogues[m_currentDialogue];

            // 设置对话内容
    m_characterLabel->setText(dialogue.character);
    m_textLabel->setText(dialogue.text);

            // 如果有指定音乐且不为空，则切换音乐
    if (!dialogue.music.isEmpty()) {
        MusicManager::instance()->switchMusic(dialogue.music);
    }

            // 先显示对话框 - 这是关键修改，先显示对话框，让它正确定位
    m_dialogueBox->show();

    // 使用QTimer::singleShot确保对话框布局完成后再设置头像位置
    QTimer::singleShot(0, this, [this, dialogue]() {
        // 设置头像
        QPixmap avatar(QString(":/images/%1").arg(dialogue.avatar));

        // 输出调试信息
        qDebug() << "头像路径:" << QString(":/images/%1").arg(dialogue.avatar);
        qDebug() << "对话框位置:" << m_dialogueBox->geometry();

                // 确保头像显示在对话框上方的左侧，且完整可见
                // 获取可用空间尺寸
        int availableHeight = m_dialogueBox->y(); // 对话框上方可用高度
        int availableWidth = width() / 3; // 限制宽度为屏幕的1/3


            qDebug() << "头像图片加载成功，大小:" << avatar.size();
            m_avatarLabel->setText("");

                    // 计算合适的尺寸，确保完整显示
            QSize originalSize = avatar.size();
            QSize targetSize;

                    // 根据原始尺寸计算合适的缩放比例
            if (originalSize.height() > availableHeight || originalSize.width() > availableWidth) {
                // 需要缩小
                targetSize = originalSize.scaled(availableWidth, availableHeight, Qt::KeepAspectRatio);
            } else {
                // 可以直接使用原始尺寸或适当放大
                targetSize = originalSize.scaled(qMin(originalSize.width(), availableWidth),
                                                 qMin(originalSize.height(), availableHeight),
                                                 Qt::KeepAspectRatio);
            }

                    // 应用计算后的尺寸
            m_avatarLabel->setPixmap(avatar.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            m_avatarLabel->setFixedSize(targetSize); // 固定大小
            m_avatarLabel->setStyleSheet("background-color: transparent;");

                // 设置头像位置 - 左侧贴边，底部紧贴对话框顶部
        int leftMargin = 20;
        // 头像底部位置等于对话框顶部位置
        int avatarBottom = m_dialogueBox->y();
        // 头像顶部位置 = 底部位置 - 头像高度
        int topPosition = avatarBottom - m_avatarLabel->height();

                // 确保头像不会超出顶部边界
        topPosition = qMax(10, topPosition);

        qDebug() << "最终头像位置:" << leftMargin << "," << topPosition;
        qDebug() << "头像大小:" << m_avatarLabel->size();

                // 重新设置头像的父控件和位置
        m_avatarLabel->setParent(this);
        m_avatarLabel->move(leftMargin, topPosition);
        m_avatarLabel->raise();
        m_avatarLabel->show();

        // 确保整个界面都能接收鼠标点击
        setFocus();
    });
}


// 在StoryMode类中覆盖鼠标点击事件
void StoryMode::mousePressEvent(QMouseEvent *event)
{
    // 点击任意位置继续到下一个对话
    m_currentDialogue++;
    showCurrentDialogue();

    // 事件已处理
    event->accept();
}


QJsonObject StoryMode::getGameParameters() const
{
    return m_gameParams;
}

void StoryMode::onSettingsClicked()
{
    Setting::instance()->show();
}
// 修改 nextDialogue 方法，添加取消键支持
void StoryMode::nextDialogue()
{
    m_currentDialogue++;

    if (m_currentDialogue >= m_dialogues.size()) {
        // 对话结束，隐藏对话框
        m_dialogueBox->hide();

        // 发送完成信号
        emit storyCompleted(m_gameParams);
    } else {
        // 显示下一条对话
        showCurrentDialogue();
    }
}

