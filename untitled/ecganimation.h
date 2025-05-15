// ecganimation.h
#ifndef ECGANIMATION_H
#define ECGANIMATION_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QTimer>
#include <QLabel>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>

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

    qreal amplitude() const { return m_amplitude; }
    void setAmplitude(qreal value);

    qreal frequency() const { return m_frequency; }
    void setFrequency(qreal value);

    qreal heartRate() const { return m_heartRate; }
    void setHeartRate(qreal value);

    qreal spO2() const { return m_spO2; }
    void setSpO2(qreal value);

    qreal bloodPressure() const { return m_bloodPressure; }
    void setBloodPressure(qreal value);

signals:
    void animationFinished();

private slots:
    void updateWaveform();
    void updateDisplay();

private:
    void setupUI();
    void setupAnimation();
    QPainterPath generateECGPath(qreal amplitude, qreal frequency);

    QGraphicsView *m_view;
    QGraphicsScene *m_scene;
    QGraphicsPathItem *m_ecgPath;

    QLabel *m_heartRateLabel;
    QLabel *m_spO2Label;
    QLabel *m_bpLabel;
    QLabel *m_timeLabel;

    QTimer *m_updateTimer;
    QTimer *m_displayTimer;
    QTimer *m_heartbeatTimer;  // 新增心跳音效计时器
    int m_currentPhase;        // 当前阶段 (1=正常, 2=警告, 3=死亡)


    QSequentialAnimationGroup *m_mainAnimation;

    qreal m_amplitude;
    qreal m_frequency;
    qreal m_heartRate;
    qreal m_spO2;
    qreal m_bloodPressure;

    QList<QPointF> m_pointHistory;
    int m_gridSize;
    void updateHeartbeatTimer();  // 更新心跳音效计时器
    void playHeartbeatSound();
};

#endif // ECGANIMATION_H
