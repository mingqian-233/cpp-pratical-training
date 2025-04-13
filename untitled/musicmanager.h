#ifndef MUSICMANAGER_H
#define MUSICMANAGER_H

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QObject>
#include <QUrl>
#include <QDebug>
#include <QList>

class MusicManager : public QObject
{
    Q_OBJECT

public:

    static MusicManager* instance();

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

    QList<QMediaPlayer*> effectPlayers;
    QList<QAudioOutput*> effectOutputs;


    double masterVolume;  // 主音量（0.0 ~ 1.0）
    double musicVolume;   // 音乐音量（0.0 ~ 1.0）
    double effectVolume;  // 音效音量（0.0 ~ 1.0）


            // 更新音量
    void updateVolume();

    void cleanupEffects();  // 新增：清理播放完的音效播放器


public slots:
    void playEffect(const QString &effectPath);

signals:
    void musicStateChanged(bool isPlaying);
    void volumeChanged(double master, double music, double effect);
};

#endif // MUSICMANAGER_H
