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
    void saveSettings();
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

private slots:
    void onReturnToMainClicked();
};

#endif


