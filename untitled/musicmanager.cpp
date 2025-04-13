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

    //无限循环
    musicPlayer->setLoops(QMediaPlayer::Infinite);
}

void MusicManager::playEffect(const QString &se)
{
    // 构建音效文件的路径
    QString effectPath = QString(":/music/se/%1").arg(se);

            // 创建一个新的播放器和输出，用于短暂音效播放
    QMediaPlayer* effectPlayer = new QMediaPlayer(this);
    QAudioOutput* effectOutput = new QAudioOutput(this);

    effectPlayers.append(effectPlayer);
    effectOutputs.append(effectOutput);

    effectPlayer->setAudioOutput(effectOutput);
    effectOutput->setVolume(masterVolume * effectVolume);

    effectPlayer->setSource(QUrl::fromLocalFile(effectPath));
    effectPlayer->play();

            // 当播放完成后自动清理
    connect(effectPlayer, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia || status == QMediaPlayer::InvalidMedia) {
            effectPlayers.removeOne(effectPlayer);
            effectOutputs.removeOne(effectOutput);
            effectPlayer->deleteLater();
            effectOutput->deleteLater();
        }
    });
}

void MusicManager::cleanupEffects()
{
    for (QMediaPlayer* player : std::as_const(effectPlayers)) {
        player->stop();
        player->deleteLater();
    }
    for (QAudioOutput* output : std::as_const(effectOutputs)) {
        output->deleteLater();
    }
    effectPlayers.clear();
    effectOutputs.clear();
}

MusicManager* MusicManager::instance()
{
    static MusicManager music;  // 局部静态变量，程序运行期间只会创建一次
    return &music;
}

MusicManager::~MusicManager()
{
    delete musicPlayer;
    delete audioOutput;
    cleanupEffects();
}

void MusicManager::setMusic(const QString &musicName)
{
    // 拼接资源路径
    QString fullPath = QString("qrc:/music/bgm/%1").arg(musicName);

            // 设置资源路径
    musicPlayer->setSource(QUrl(fullPath));
}


void MusicManager::playMusic()
{
    if (musicPlayer->source().isEmpty()) {
        qDebug() << "Error: No music file set.";
        return;
    }

            // 播放音乐
    audioOutput->setVolume(musicVolume);
     // 将音量转换为 0~100 整数
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

void MusicManager::updateVolume()
{
    // 更新音乐播放器的音量
    audioOutput->setVolume(masterVolume*  musicVolume);
}

