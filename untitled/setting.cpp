#include "setting.h"
#include "musicmanager.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include <QApplication>
#include <QScreen>

// 初始化静态成员变量
Setting* Setting::m_instance = nullptr;

void Setting::initialize(QWidget *parent)
{
    if (!m_instance) {
        m_instance = new Setting(parent);
    }
}

Setting* Setting::instance()
{
    if (!m_instance) {
        // 如果没有初始化，使用nullptr作为父窗口
        m_instance = new Setting(nullptr);
    }
    return m_instance;
}

Setting::Setting(QWidget *parent)
    : QDialog(parent)
      , isMute(false)
      , isFullScreen(false)  // 默认为非全屏
{
    setWindowTitle("设置");
    setupUI();

    // 连接信号槽
    connect(muteCheckBox, &QCheckBox::toggled, this, &Setting::onMuteToggled);
    connect(masterSlider, &QSlider::valueChanged, this, &Setting::onMasterVolumeChanged);
    connect(musicSlider, &QSlider::valueChanged, this, &Setting::onMusicVolumeChanged);
    connect(effectSlider, &QSlider::valueChanged, this, &Setting::onEffectVolumeChanged);
    connect(fullscreenCheckBox, &QCheckBox::toggled, this, &Setting::onFullscreenToggled);
}

void Setting::setupUI()
{
    muteCheckBox = new QCheckBox("静音");
    muteCheckBox->setChecked(isMute);

    fullscreenCheckBox = new QCheckBox("全屏");
    fullscreenCheckBox->setChecked(isFullScreen);

    masterSlider = new QSlider(Qt::Horizontal);
    musicSlider = new QSlider(Qt::Horizontal);
    effectSlider = new QSlider(Qt::Horizontal);

    masterSlider->setRange(0, 100);
    musicSlider->setRange(0, 100);
    effectSlider->setRange(0, 100);

            // 设置初始值
    masterSlider->setValue(100);
    musicSlider->setValue(100);
    effectSlider->setValue(100);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel("音乐"));
    layout->addWidget(muteCheckBox);
    layout->addWidget(new QLabel("主音量"));
    layout->addWidget(masterSlider);
    layout->addWidget(new QLabel("背景音乐"));
    layout->addWidget(musicSlider);
    layout->addWidget(new QLabel("音效"));
    layout->addWidget(effectSlider);
    layout->addWidget(new QLabel("窗口"));
    layout->addWidget(fullscreenCheckBox);
    setLayout(layout);
}

void Setting::setIsMute(bool mute)
{
    isMute = mute;
    if (muteCheckBox) {
        muteCheckBox->setChecked(mute);
    }
}

void Setting::setIsFullScreen(bool fullscreen)
{
    isFullScreen = fullscreen;
    if (fullscreenCheckBox) {
        fullscreenCheckBox->setChecked(fullscreen);
    }
}

void Setting::applySettings()
{
    // 应用所有设置
    onMuteToggled(isMute);
    onMasterVolumeChanged(masterSlider->value());
    onMusicVolumeChanged(musicSlider->value());
    onEffectVolumeChanged(effectSlider->value());
    onFullscreenToggled(isFullScreen);
}

void Setting::onMuteToggled(bool checked)
{
    isMute = checked;
    MusicManager::instance()->setMasterVolume(checked ? 0.0 : masterSlider->value() / 100.0);
}

void Setting::onMasterVolumeChanged(int value)
{
    if (!isMute) {
        MusicManager::instance()->setMasterVolume(value / 100.0);
    }
}

void Setting::onMusicVolumeChanged(int value)
{
    MusicManager::instance()->setMusicVolume(value / 100.0);
}

void Setting::onEffectVolumeChanged(int value)
{
    MusicManager::instance()->setEffectVolume(value / 100.0);
}

void Setting::onFullscreenToggled(bool checked)
{
    isFullScreen = checked;
    MainWindow* mainWindow = MainWindow::instance();

    if (mainWindow) {
        if (checked) {
            mainWindow->showFullScreen();
        } else {
            mainWindow->showNormal();
        }
    }
}
