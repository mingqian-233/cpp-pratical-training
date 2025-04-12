#include "musicmanager.h"
#include <QUrl>

MusicManager::MusicManager(QObject *parent)
    : QObject(parent)
      , musicPlayer(new QMediaPlayer(this))
      , audioOutput(new QAudioOutput(this))
      , masterVolume(1.0)  // 默认主音量为最大
      , musicVolume(1.0)   // 默认音乐音量为最大
      , effectVolume(1.0)  // 默认音效音量为最大
{
    // 将音频输出与播放器关联
    musicPlayer->setAudioOutput(audioOutput);

            // 连接音乐播放完毕信号
    connect(musicPlayer, &QMediaPlayer::positionChanged, this, &MusicManager::onMusicFinished);
}

MusicManager::~MusicManager()
{
    delete musicPlayer;
    delete audioOutput;
}

void MusicManager::setMusic(const QString &musicPath)
{
    // 设置音乐文件路径
    musicPlayer->setSource(QUrl::fromLocalFile(musicPath));
}

void MusicManager::playMusic()
{
    if (musicPlayer->source().isEmpty()) {
        qDebug() << "Error: No music file set.";
        return;
    }

            // 播放音乐
    musicPlayer->setVolume(static_cast<int>(musicVolume * 100)); // 将音量转换为 0~100 整数
    musicPlayer->play();

            // 触发播放状态信号
    emit musicStateChanged(true);
}

void MusicManager::pauseMusic()
{
    // 暂停音乐
    musicPlayer->pause();
    emit musicStateChanged(false);
}

void MusicManager::switchMusic(const QString &newMusicPath)
{
    // 停止当前播放的音乐
    pauseMusic();

            // 设置新的音乐文件
    setMusic(newMusicPath);

            // 开始播放新的音乐
    playMusic();
}

void MusicManager::setMasterVolume(double volume)
{
    // 确保音量在合理范围内 [0.0, 1.0]
    masterVolume = qBound(0.0, volume, 1.0);
    updateVolume();
    emit volumeChanged(masterVolume, musicVolume, effectVolume);
}

void MusicManager::setMusicVolume(double volume)
{
    // 确保音量在合理范围内 [0.0, 1.0]
    musicVolume = qBound(0.0, volume, 1.0);
    updateVolume();
    emit volumeChanged(masterVolume, musicVolume, effectVolume);
}

void MusicManager::setEffectVolume(double volume)
{
    // 确保音量在合理范围内 [0.0, 1.0]
    effectVolume = qBound(0.0, volume, 1.0);
    emit volumeChanged(masterVolume, musicVolume, effectVolume);
}

double MusicManager::calculateTotalVolume() const
{
    // 计算总音量 = 主音量 × 音乐音量 × 音效音量
    return masterVolume * musicVolume * effectVolume;
}

void MusicManager::updateVolume()
{
    // 更新音乐播放器的音量
    musicPlayer->setVolume(static_cast<int>(calculateTotalVolume() * 100));
}

void MusicManager::onMusicFinished()
{
    // 如果音乐播放完毕，可以在这里处理切换逻辑
    qDebug() << "Music Finished";
}
