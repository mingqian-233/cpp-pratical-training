#include "setting.h"
#include "musicmanager.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include <QApplication>
#include <QScreen>
#include <QSettings>
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
// 添加保存设置的函数
void Setting::saveSettings()
{
    QSettings settings("mingqian233", "HerbalOdyssey");
    settings.setValue("audio/mute", isMute);
    settings.setValue("audio/masterVolume", masterSlider->value());
    settings.setValue("audio/musicVolume", musicSlider->value());
    settings.setValue("audio/effectVolume", effectSlider->value());
    settings.setValue("video/fullscreen", isFullScreen);
}

Setting::Setting(QWidget *parent)
    : QDialog(parent)
      , isMute(false)
      , isFullScreen(false)  // 默认为非全屏
{
    setWindowTitle("设置");

    // 从配置文件读取设置
    QSettings settings("mingqian233", "HerbalOdyssey");
    isMute = settings.value("audio/mute", false).toBool();
    isFullScreen = settings.value("video/fullscreen", true).toBool(); // 默认全屏

    setupUI();

    // 设置初始值
    muteCheckBox->setChecked(isMute);
    fullscreenCheckBox->setChecked(isFullScreen);
    masterSlider->setValue(settings.value("audio/masterVolume", 100).toInt());
    musicSlider->setValue(settings.value("audio/musicVolume", 100).toInt());
    effectSlider->setValue(settings.value("audio/effectVolume", 100).toInt());

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

    // 创建返回主菜单按钮
    QPushButton *returnToMainButton = new QPushButton("返回主菜单");
    returnToMainButton->setMinimumSize(120, 30);
    connect(returnToMainButton, &QPushButton::clicked, this, &Setting::onReturnToMainClicked);

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
    layout->addSpacing(20);
    layout->addWidget(returnToMainButton, 0, Qt::AlignCenter);
    setLayout(layout);
}


// 添加返回主菜单按钮处理函数
void Setting::onReturnToMainClicked()
{
    // 应用当前设置
    applySettings();
    MainWindow::instance()->switchToMainPage();

    // 关闭设置对话框
    close();
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
            // 恢复窗口大小并居中
            mainWindow->resize(1280, 720);
            QScreen *screen = QGuiApplication::primaryScreen();
            if (screen) {
                QRect screenGeometry = screen->availableGeometry();
                int x = (screenGeometry.width() - mainWindow->width()) / 2;
                int y = (screenGeometry.height() - mainWindow->height()) / 2;
                mainWindow->move(x, y);
            }
        }
    }

    saveSettings(); // 保存设置
}
