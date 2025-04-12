#ifndef MUSICMANAGER_H
#define MUSICMANAGER_H

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QObject>
#include <QUrl>
#include <QDebug>

class MusicManager : public QObject
{
    Q_OBJECT

public:
    explicit MusicManager(QObject *parent = nullptr);
    ~MusicManager();

            // 接口：设置当前播放的音乐文件
    void setMusic(const QString &musicPath);

            // 接口：播放音乐
    void playMusic();

            // 接口：暂停音乐
    void pauseMusic();

            // 接口：切换音乐
    void switchMusic(const QString &newMusicPath);

            // 接口：设置主音量
    void setMasterVolume(double volume);

            // 接口：设置音乐音量
    void setMusicVolume(double volume);

            // 接口：设置音效音量
    void setEffectVolume(double volume);

private:
    QMediaPlayer *musicPlayer;
    QAudioOutput *audioOutput;
    double masterVolume;  // 主音量（0.0 ~ 1.0）
    double musicVolume;   // 音乐音量（0.0 ~ 1.0）
    double effectVolume;  // 音效音量（0.0 ~ 1.0）

            // 计算总音量
    double calculateTotalVolume() const;

            // 更新音量
    void updateVolume();

public slots:
    void onMusicFinished();

signals:
    void musicStateChanged(bool isPlaying);
    void volumeChanged(double master, double music, double effect);
};

#endif // MUSICMANAGER_H
