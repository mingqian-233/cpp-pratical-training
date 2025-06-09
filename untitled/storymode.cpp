#include "storymode.h"
#include "musicmanager.h"
#include "qapplication.h"
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
#include "ecganimation.h"
#include "spacetimevortexanimation.h"


StoryMode::StoryMode(QWidget *parent) : QWidget(parent),
                                        m_isAnimationPlaying(false),
                                        m_currentPhase(StoryPhase::PreGame),  // 新增
                                        m_currentDialogues(&m_preGameDialogues),
                                        m_currentDialogue(0)// 新增
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
    m_currentChapterNumber = chapter;
    qDebug()<<"m_currentChapterNumber："<<m_currentChapterNumber;

            // 重置状态
    m_currentDialogue = 0;
    m_currentPhase = StoryPhase::PreGame;

            // 加载章节文件
    if (!loadChapterFile(chapter)) {
        return false;
    }

            // 更新背景
    m_backgroundWidget->setBackground(m_background);

            // 使用新的章节过渡效果
    m_chapterTransition->resize(size());
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

            // 清空之前的对话数据
    m_preGameDialogues.clear();
    m_postGameDialogues.clear();
    m_dialogues.clear(); // 保留兼容性

            // 解析游戏前对话数据
    if (chapterData.contains("pre_game_dialogues")) {
        QJsonArray preDialoguesArray = chapterData["pre_game_dialogues"].toArray();
        for (int i = 0; i < preDialoguesArray.size(); i++) {
            QJsonObject dialogueObj = preDialoguesArray[i].toObject();
            DialogueData dialogue;
            dialogue.character = dialogueObj["character"].toString();
            dialogue.avatar = dialogueObj["avatar"].toString();
            dialogue.text = dialogueObj["text"].toString();
            dialogue.music = dialogueObj["music"].toString();
            dialogue.background = dialogueObj["background"].toString();
            dialogue.sfx = dialogueObj["sfx"].toString();

            m_preGameDialogues.append(dialogue);
        }
    }

            // 解析游戏后对话数据
    if (chapterData.contains("post_game_dialogues")) {
        QJsonArray postDialoguesArray = chapterData["post_game_dialogues"].toArray();
        for (int i = 0; i < postDialoguesArray.size(); i++) {
            QJsonObject dialogueObj = postDialoguesArray[i].toObject();
            DialogueData dialogue;
            dialogue.character = dialogueObj["character"].toString();
            dialogue.avatar = dialogueObj["avatar"].toString();
            dialogue.text = dialogueObj["text"].toString();
            dialogue.music = dialogueObj["music"].toString();
            dialogue.sfx = dialogueObj["sfx"].toString();
            dialogue.background = dialogueObj["background"].toString();
            m_postGameDialogues.append(dialogue);
        }
    }

            // 解析游戏参数
    m_gameParams = chapterData["game"].toObject();

            // 设置初始状态
    m_currentPhase = StoryPhase::PreGame;
    m_currentDialogues = &m_preGameDialogues;
    m_currentDialogue = 0;

    return true;
}

void StoryMode::showCurrentDialogue()
{
    // 检查当前对话数组是否为空或越界
    if (!m_currentDialogues || m_currentDialogue >= m_currentDialogues->size()) {
        // 当前阶段的对话已完成
        if (m_currentPhase == StoryPhase::PreGame) {
            // 游戏前对话完成，准备开始游戏
            m_currentPhase = StoryPhase::Gaming;
            m_dialogueBox->hide();
            m_avatarLabel->hide();
            emit storyCompleted(m_gameParams);
            return;
        } else if (m_currentPhase == StoryPhase::PostGame) {
            // 游戏后对话完成，章节彻底结束
            m_dialogueBox->hide();
            m_avatarLabel->hide();
            emit chapterCompleted(); // 新增信号，表示整个章节完成
            return;
        }
    }

            // 获取当前对话
    const DialogueData& dialogue = (*m_currentDialogues)[m_currentDialogue];

            // 设置对话内容
    m_characterLabel->setText(dialogue.character);
    m_textLabel->setText(dialogue.text);

            // 如果有指定音乐且不为空，则切换音乐
    if (!dialogue.music.isEmpty()) {
        MusicManager::instance()->switchMusic(dialogue.music);
    }
    if (!dialogue.sfx.isEmpty()) {
        MusicManager::instance()->playEffect(dialogue.sfx);
    }
    // 如果有指定背景且不为空，则切换背景
    if (!dialogue.background.isEmpty()) {
        m_backgroundWidget->setBackground(dialogue.background);
    }

            // 先显示对话框
    m_dialogueBox->show();

    // 强制更新布局，确保对话框尺寸正确
    m_dialogueBox->updateGeometry();
    m_dialogueBox->layout()->activate();
    QApplication::processEvents(); // 确保布局完全更新

            // 设置头像
    QPixmap avatar(QString(":/images/%1").arg(dialogue.avatar));

    int availableHeight = m_dialogueBox->y();
    int availableWidth = width() / 3;

    m_avatarLabel->setText("");

    QSize originalSize = avatar.size();
    QSize targetSize;

    if (originalSize.height() > availableHeight || originalSize.width() > availableWidth) {
        targetSize = originalSize.scaled(availableWidth, availableHeight, Qt::KeepAspectRatio);
    } else {
        targetSize = originalSize.scaled(qMin(originalSize.width(), availableWidth),
                                         qMin(originalSize.height(), availableHeight),
                                         Qt::KeepAspectRatio);
    }

    m_avatarLabel->setPixmap(avatar.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_avatarLabel->setFixedSize(targetSize);
    m_avatarLabel->setStyleSheet("background-color: transparent;");

            // 现在计算头像位置（此时对话框布局已经更新）
    updateAvatarPosition();

    m_avatarLabel->show();
    setFocus();
}

void StoryMode::mousePressEvent(QMouseEvent *event)
{
    // 如果动画正在播放，忽略鼠标事件
    if (m_isAnimationPlaying) {
        event->accept();
        return;
    }

    nextDialogue();
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

// 修改后的 nextDialogue 方法
void StoryMode::nextDialogue()
{
    m_currentDialogue++;
    qDebug()<<"当前对话"<<m_currentDialogue;

    if (!m_currentDialogues || m_currentDialogue >= m_currentDialogues->size()) {
        if (m_currentPhase == StoryPhase::PreGame) {
            m_dialogueBox->hide();
            m_avatarLabel->hide();

            if (m_currentChapterNumber == 1) {
                // 设置动画播放标志，冻结鼠标操作
                m_isAnimationPlaying = true;

                ECGAnimation *ecgAnimation = new ECGAnimation(this);
                ecgAnimation->resize(this->size());
                ecgAnimation->show();
                ecgAnimation->raise();
                ecgAnimation->startAnimation();

                connect(ecgAnimation, &ECGAnimation::animationFinished, this, [this, ecgAnimation]() {
                    ecgAnimation->deleteLater();

                    // 第一个动画完成，开始第二个动画
                    SpaceTimeVortexAnimation *vortexAnimation = new SpaceTimeVortexAnimation(this);
                    vortexAnimation->resize(this->size());
                    vortexAnimation->show();
                    vortexAnimation->raise();
                    vortexAnimation->startAnimation();
                    connect(vortexAnimation, &SpaceTimeVortexAnimation::animationFinished, this, [this, vortexAnimation]() {
                        vortexAnimation->deleteLater();
                        // 所有动画完成，恢复鼠标操作
                        m_currentPhase = StoryPhase::Gaming;
                        emit storyCompleted(m_gameParams);
                    });
                });
            } else {
                m_currentPhase = StoryPhase::Gaming;
                emit storyCompleted(m_gameParams);
            }
            return;
        } else if (m_currentPhase == StoryPhase::PostGame) {
            m_dialogueBox->hide();
            m_avatarLabel->hide();
            emit chapterCompleted();
            return;
        }
    } else {
        showCurrentDialogue();
    }
}
void StoryMode::onGameCompleted()
{
    if (m_postGameDialogues.isEmpty()) {
        // 没有游戏后对话，直接完成章节
        emit chapterCompleted();
        return;
    }

            // 有游戏后对话，切换到游戏后阶段
    m_currentPhase = StoryPhase::PostGame;
    m_currentDialogues = &m_postGameDialogues;
    m_currentDialogue = 0;

    // 显示游戏后的第一条对话
    showCurrentDialogue();
}

// 新增方法：更新头像位置
void StoryMode::updateAvatarPosition()
{
    if (!m_avatarLabel || !m_dialogueBox || !m_dialogueBox->isVisible()) {
        return;
    }

            // 确保布局是最新的
    m_dialogueBox->layout()->activate();

    int leftMargin = 20;
    int avatarBottom = m_dialogueBox->y();
    int topPosition = avatarBottom - m_avatarLabel->height();
    topPosition = qMax(10, topPosition);

            // 限制横向不超出边界
    int maxX = width() - m_avatarLabel->width() - 10;
    int x = qMin(leftMargin, maxX);

    m_avatarLabel->setParent(this);
    m_avatarLabel->move(x, topPosition);
    m_avatarLabel->raise();
}

// 修改 resizeEvent 方法:
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

            // 如果当前对话框和头像都在显示，则重新计算头像位置
    if (m_dialogueBox && m_dialogueBox->isVisible() &&
        m_avatarLabel && m_avatarLabel->isVisible()) {
        // 使用延迟更新，确保窗口大小调整完成
        QTimer::singleShot(0, this, [this]() {
            updateAvatarPosition();
        });
    }
}
