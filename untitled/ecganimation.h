#ifndef ECGANIMATION_H
#define ECGANIMATION_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QAudioOutput>

class ECGAnimation : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal amplitude READ amplitude WRITE setAmplitude)
    Q_PROPERTY(qreal frequency READ frequency WRITE setFrequency)
    Q_PROPERTY(qreal heartRate READ heartRate WRITE setHeartRate)
    Q_PROPERTY(qreal spO2 READ spO2 WRITE setSpO2)
    Q_PROPERTY(qreal bloodPressure READ bloodPressure WRITE setBloodPressure)

public:
    explicit ECGAnimation(QWidget *parent = nullptr);
    ~ECGAnimation();

    void startAnimation();
    void skipAnimation();

            // 属性访问器
    qreal amplitude() const { return m_amplitude; }
    qreal frequency() const { return m_frequency; }
    qreal heartRate() const { return m_heartRate; }
    qreal spO2() const { return m_spO2; }
    qreal bloodPressure() const { return m_bloodPressure; }

public slots:
    void setAmplitude(qreal value);
    void setFrequency(qreal value);
    void setHeartRate(qreal value);
    void setSpO2(qreal value);
    void setBloodPressure(qreal value);

signals:
    void animationFinished();

private slots:
    void updateDisplay();
    void triggerHeartbeat();
    void onPhaseChanged();
    void onVideoPositionChanged(qint64 position);

private:
    void setupUI();
    void setupAnimation();
    void setupVideo();
    void updateHeartbeatTimer();
    void updateDisplayColors();
    void playHeartbeatSound();
    void updateVideoForPhase();

            // UI组件
    QVideoWidget *m_videoWidget;        // 视频播放器
    QMediaPlayer *m_mediaPlayer;        // 媒体播放器
    QAudioOutput *m_audioOutput;        // 音频输出
    QLabel *m_heartRateLabel;
    QLabel *m_spO2Label;
    QLabel *m_bpLabel;
    QLabel *m_timeLabel;

            // 动画和定时器
    QSequentialAnimationGroup *m_mainAnimation;
    QTimer *m_displayTimer;             // 显示更新定时器
    QTimer *m_heartbeatTimer;           // 心跳触发定时器

            // 基本参数
    qreal m_amplitude;
    qreal m_frequency;
    qreal m_heartRate;
    qreal m_spO2;
    qreal m_bloodPressure;
    int m_currentPhase;
    bool m_isFlatlined;

            // 视频控制参数
    QString m_videoPath;                // 视频文件路径
    qint64 m_normalStart;              // 正常阶段开始时间 (0s)
    qint64 m_normalEnd;                // 正常阶段结束时间 (5s)
    qint64 m_abnormalStart;            // 异常阶段开始时间 (5s)
    qint64 m_abnormalEnd;              // 异常阶段结束时间 (10s)
    qint64 m_deathStart;               // 死亡阶段开始时间 (10s)
    qint64 m_deathEnd;                 // 死亡阶段结束时间 (16s)
};

#endif // ECGANIMATION_H
