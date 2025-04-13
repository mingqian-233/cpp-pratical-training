#ifndef SETTING_H
#define SETTING_H

#include <QDialog>

class QCheckBox;
class QSlider;

class Setting : public QDialog
{
    Q_OBJECT

public:
    // 初始化单例
    static void initialize(QWidget *parent = nullptr);

    // 获取单例实例
    static Setting* instance();

    // 禁止拷贝构造和赋值操作
    Setting(const Setting&) = delete;
    Setting& operator=(const Setting&) = delete;

    // 获取状态
    bool getIsMute() const { return isMute; }
    bool getIsFullScreen() const { return isFullScreen; }

    // 设置状态
    void setIsMute(bool mute);
    void setIsFullScreen(bool fullscreen);

    // 应用设置
    void applySettings();

private slots:
    void onMuteToggled(bool checked);
    void onMasterVolumeChanged(int value);
    void onMusicVolumeChanged(int value);
    void onEffectVolumeChanged(int value);
    void onFullscreenToggled(bool checked);

private:
    explicit Setting(QWidget *parent = nullptr);
    static Setting* m_instance;

    QCheckBox *muteCheckBox;
    QSlider *masterSlider;
    QSlider *musicSlider;
    QSlider *effectSlider;
    QCheckBox *fullscreenCheckBox;

    bool isMute;
    bool isFullScreen;

    // 初始化UI
    void setupUI();
};

#endif

/*
常见使用场景

1. 创建新游戏
// 创建新存档，设置玩家名称为"PlayerOne"
saveManager->createNewSave("PlayerOne");
saveManager->saveSaveFile();

2. 保存游戏进度
// 更新故事模式进度
saveManager->setStoryProgress(5);
// 更新自由模式高分
saveManager->setFreeHighScore(2000);
// 保存到文件
saveManager->saveSaveFile();

3. 加载游戏
if (saveManager->loadSaveFile()) {
 // 读取存档信息
 QString playerName = saveManager->getPlayerName();
 int storyProgress = saveManager->getStoryProgress();
 int freeProgress = saveManager->getFreeProgress();
 int highScore = saveManager->getFreeHighScore();

// 根据读取的信息设置游戏状态
// ...
}

4. 使用不同的存档文件
// 加载指定名称的存档
saveManager->loadSaveFile("player2_save.json");

// 保存到指定名称的存档
saveManager->saveSaveFile("backup_save.json");

 */
