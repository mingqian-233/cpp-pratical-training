#ifndef STORYMODE_H
#define STORYMODE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <QTimer>
#include "backgroundwidget.h"
#include "ChapterTransition.h"

// 对话数据结构
struct DialogueData {
    QString character;
    QString avatar;
    QString text;
    QString music;  // 新增音乐字段，存储音乐文件名
};

class StoryMode : public QWidget
{
    Q_OBJECT

public:
    explicit StoryMode(QWidget *parent = nullptr);
    ~StoryMode();

            // 加载指定章节
    bool loadChapter(int chapter);

    // 获取当前章节游戏参数
    QJsonObject getGameParameters() const;

signals:
    // 剧情完成，准备开始游戏
    void storyCompleted(QJsonObject gameParams);
    // 返回主菜单信号
    void returnToMainMenu();

private slots:
    void nextDialogue();
    void onSettingsClicked();

private:
    // 初始化UI
    void setupUI();
    // 显示当前对话
    void showCurrentDialogue();
    // 加载章节文件
    bool loadChapterFile(int chapter);
    // 在 StoryMode 类的 private 部分添加：
    void mousePressEvent(QMouseEvent *event);
    // 章节标题
    QString m_chapterTitle;
    // 背景图片名称
    QString m_background;
    // 对话数据列表
    QVector<DialogueData> m_dialogues;
    // 当前对话索引
    int m_currentDialogue;
    // 游戏参数
    QJsonObject m_gameParams;

            // UI组件
    BackgroundWidget* m_backgroundWidget;
    QWidget* m_dialogueBox;
    QLabel* m_avatarLabel;
    QLabel* m_characterLabel;
    QLabel* m_textLabel;
    QPushButton* m_nextButton;
    QPushButton* m_settingsButton;

private slots:
    void onChapterTransitionFinished();

private:
    // ... 其他已有的成员变量 ...
    ChapterTransition* m_chapterTransition;

    // 在 private: 部分如果还没有 resizeEvent 函数声明，添加以下内容
protected:
    // ... 其他已有的保护成员函数 ...
    void resizeEvent(QResizeEvent *event) override;
private:
    QPixmap m_currentAvatar;   // 存储原始头像
    void adjustAvatar();
};

#endif // STORYMODE_H
