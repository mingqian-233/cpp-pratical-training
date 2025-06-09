#ifndef STORYMODE_H
#define STORYMODE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <QMouseEvent>
#include "backgroundwidget.h"
#include "chaptertransition.h"

// 对话数据结构
struct DialogueData {
    QString character;
    QString avatar;
    QString text;
    QString music;
    QString sfx;
    QString background;
};

// 故事阶段枚举 - 新增
enum class StoryPhase {
    PreGame,    // 游戏前对话
    Gaming,     // 游戏进行中
    PostGame    // 游戏后对话
};

class StoryMode : public QWidget
{
    Q_OBJECT
private:
    bool m_isAnimationPlaying=false; // 动画播放标志
public:
    explicit StoryMode(QWidget *parent = nullptr);
    ~StoryMode();

    bool loadChapter(int chapter);
    QJsonObject getGameParameters() const;

    // 新增公共方法
    void onGameCompleted();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void nextDialogue();
    void onSettingsClicked();
    void onChapterTransitionFinished();

signals:
    // 如果这些信号已经存在，请删除重复的声明
    void storyCompleted(QJsonObject gameParams);  // 游戏前对话完成，开始游戏
    void chapterCompleted();                      // 整个章节完成（包括游戏后对话） - 新增
    void returnToMainMenu();

private:
    void setupUI();
    bool loadChapterFile(int chapter);
    void showCurrentDialogue();

            // UI组件
    BackgroundWidget* m_backgroundWidget;
    QWidget* m_dialogueBox;
    QLabel* m_characterLabel;
    QLabel* m_textLabel;
    QLabel* m_avatarLabel;
    QPushButton* m_nextButton;
    QPushButton* m_settingsButton;
    ChapterTransition* m_chapterTransition;

            // 数据成员
    QString m_chapterTitle;
    QString m_background;
    QJsonObject m_gameParams;

    // 修改后的对话系统 - 新增
    StoryPhase m_currentPhase;
    QVector<DialogueData> m_preGameDialogues;   // 游戏前对话
    QVector<DialogueData> m_postGameDialogues;  // 游戏后对话
    QVector<DialogueData>* m_currentDialogues;  // 指向当前使用的对话数组

    // 保留原有的成员变量以兼容旧代码
    QVector<DialogueData> m_dialogues;  // 兼容性保留

    int m_currentDialogue;
    int m_currentChapterNumber;
private:
    void updateAvatarPosition(); // 新增方法声明

};

#endif // STORYMODE_H
