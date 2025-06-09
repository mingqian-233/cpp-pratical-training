#include "ecganimation.h"
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtMath>
#include <QRandomGenerator>
#include <QPushButton>
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QMediaMetaData>
// #include "musicmanager.h" // 如果没有此文件请注释掉

ECGAnimation::ECGAnimation(QWidget *parent)
    : QWidget(parent),
      m_amplitude(1.0),
      m_frequency(1.0),
      m_heartRate(80.0),
      m_spO2(98.0),
      m_bloodPressure(120.0),
      m_currentPhase(0),
      m_isFlatlined(false),
      m_videoPath("qrc:/video/ecg.mp4"),
      m_normalStart(0),          // 0秒
      m_normalEnd(5000),         // 5秒 (毫秒)
      m_abnormalStart(5000),     // 5秒
      m_abnormalEnd(10000),      // 10秒
      m_deathStart(10000),       // 10秒
      m_deathEnd(16000)          // 16秒
{
    setupUI();
    setupVideo();
    setupAnimation();

            // 显示信息更新定时器
    m_displayTimer = new QTimer(this);
    connect(m_displayTimer, &QTimer::timeout, this, &ECGAnimation::updateDisplay);
    m_displayTimer->setInterval(1000);

            // 心跳触发定时器
    m_heartbeatTimer = new QTimer(this);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &ECGAnimation::triggerHeartbeat);
}

ECGAnimation::~ECGAnimation()
{
    if (m_displayTimer) m_displayTimer->stop();
    if (m_heartbeatTimer) m_heartbeatTimer->stop();
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
        delete m_mediaPlayer;
    }
    if (m_audioOutput) {
        delete m_audioOutput;
    }
    delete m_mainAnimation;
}

void ECGAnimation::setupUI()
{
    setStyleSheet("background-color: black;");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);

            // 顶部信息栏
    QHBoxLayout *infoLayout = new QHBoxLayout();
    m_heartRateLabel = new QLabel("HR: 80", this);
    m_heartRateLabel->setStyleSheet("color: #00FF00; font-size: 24px; font-weight: bold; font-family: 'Courier New';");
    m_spO2Label = new QLabel("SpO₂: 98%", this);
    m_spO2Label->setStyleSheet("color: #00FFFF; font-size: 24px; font-weight: bold; font-family: 'Courier New';");
    m_bpLabel = new QLabel("BP: 120/80", this);
    m_bpLabel->setStyleSheet("color: #FF8800; font-size: 24px; font-weight: bold; font-family: 'Courier New';");
    m_timeLabel = new QLabel(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), this);
    m_timeLabel->setStyleSheet("color: white; font-size: 24px; font-family: 'Courier New';");

    infoLayout->addWidget(m_heartRateLabel);
    infoLayout->addWidget(m_spO2Label);
    infoLayout->addWidget(m_bpLabel);
    infoLayout->addStretch();
    infoLayout->addWidget(m_timeLabel);
    mainLayout->addLayout(infoLayout);

            // 视频播放区域
    m_videoWidget = new QVideoWidget(this);
    m_videoWidget->setStyleSheet("background-color: #001100; border: 2px solid #333333; border-radius: 5px;");
    m_videoWidget->setFixedSize(800, 400);

    mainLayout->addWidget(m_videoWidget);
    mainLayout->setAlignment(m_videoWidget, Qt::AlignCenter);

            // 底部信息
    QLabel *deviceInfoLabel = new QLabel("医疗监护系统 v3.0 | 重症监护室 | 病床号: 713", this);
    deviceInfoLabel->setStyleSheet("color: #888888; font-size: 18px; font-family: 'Courier New';");
    deviceInfoLabel->setAlignment(Qt::AlignRight);
    mainLayout->addWidget(deviceInfoLabel);

    QPushButton *skipButton = new QPushButton("跳过动画", this);
    skipButton->setStyleSheet("QPushButton { background-color: #333333; color: white; "
        "border: 1px solid #555555; padding: 8px; border-radius: 4px; font-family: 'Courier New'; }"
        "QPushButton:hover { background-color: #444444; }");
    connect(skipButton, &QPushButton::clicked, this, &ECGAnimation::skipAnimation);
    mainLayout->addWidget(skipButton, 0, Qt::AlignRight);
}
void ECGAnimation::setupVideo()
{
    m_mediaPlayer = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_mediaPlayer->setAudioOutput(m_audioOutput);
    m_mediaPlayer->setVideoOutput(m_videoWidget);


    // 📊 详细调试 - 找出具体问题
    QString videoPath = "qrc:/video/ecg.mp4";

    qDebug() << "🎬 视频调试开始";
    qDebug() << "视频路径:" << videoPath;

    // 检查资源是否存在
    QFile resourceFile(videoPath);
    if (resourceFile.exists()) {
        qDebug() << "✅ 资源文件存在，大小:" << resourceFile.size() << "字节";
    } else {
        qWarning() << "❌ 资源文件不存在!";

        // 列出所有可用资源
        QDir resourceDir(":/");
        qDebug() << "可用资源根目录:" << resourceDir.entryList();

        QDir videoDir(":/video");
        qDebug() << "video 目录内容:" << videoDir.entryList();
    }

    // 设置视频源（完全按照音频的方式）
    m_mediaPlayer->setSource(QUrl(videoPath));

    // 详细错误监控
    connect(m_mediaPlayer, &QMediaPlayer::errorOccurred, this,
            [](QMediaPlayer::Error error, const QString &errorString) {
                qWarning() << "❌ 视频播放错误:" << error << errorString;
            });

    connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this,
            [this](QMediaPlayer::MediaStatus status) {
                qDebug() << "视频状态:" << status;
                if (status == QMediaPlayer::LoadedMedia) {
                    qDebug() << "✅ 视频加载成功，时长:" << m_mediaPlayer->duration() << "ms";
                } else if (status == QMediaPlayer::InvalidMedia) {
                    qWarning() << "❌ 视频无效!";
                }
            });

    // 测试：立即播放看看效果
    m_mediaPlayer->play();

}

void ECGAnimation::setupAnimation()
{
    m_mainAnimation = new QSequentialAnimationGroup(this);

            // 第一阶段：正常状态 (0-5s视频段)
    QPropertyAnimation *phase1Heart = new QPropertyAnimation(this, "heartRate");
    phase1Heart->setDuration(5000);
    phase1Heart->setStartValue(75.0);
    phase1Heart->setEndValue(120.0);
    phase1Heart->setEasingCurve(QEasingCurve::InQuad);

    QPropertyAnimation *phase1Amp = new QPropertyAnimation(this, "amplitude");
    phase1Amp->setDuration(5000);
    phase1Amp->setStartValue(1.0);
    phase1Amp->setEndValue(1.2);

    QPropertyAnimation *phase1SpO2 = new QPropertyAnimation(this, "spO2");
    phase1SpO2->setDuration(5000);
    phase1SpO2->setStartValue(98.0);
    phase1SpO2->setEndValue(95.0);

    QPropertyAnimation *phase1BP = new QPropertyAnimation(this, "bloodPressure");
    phase1BP->setDuration(5000);
    phase1BP->setStartValue(120.0);
    phase1BP->setEndValue(130.0);

    QParallelAnimationGroup *phase1 = new QParallelAnimationGroup();
    phase1->addAnimation(phase1Heart);
    phase1->addAnimation(phase1Amp);
    phase1->addAnimation(phase1SpO2);
    phase1->addAnimation(phase1BP);

            // 第二阶段：异常状态 (5-10s视频段)
    QPropertyAnimation *phase2Heart = new QPropertyAnimation(this, "heartRate");
    phase2Heart->setDuration(5000);
    phase2Heart->setStartValue(120.0);
    phase2Heart->setEndValue(180.0);
    phase2Heart->setEasingCurve(QEasingCurve::InCubic);

    QPropertyAnimation *phase2Amp = new QPropertyAnimation(this, "amplitude");
    phase2Amp->setDuration(5000);
    phase2Amp->setStartValue(1.2);
    phase2Amp->setEndValue(1.6);

    QPropertyAnimation *phase2SpO2 = new QPropertyAnimation(this, "spO2");
    phase2SpO2->setDuration(5000);
    phase2SpO2->setStartValue(95.0);
    phase2SpO2->setEndValue(80.0);

    QPropertyAnimation *phase2BP = new QPropertyAnimation(this, "bloodPressure");
    phase2BP->setDuration(5000);
    phase2BP->setStartValue(130.0);
    phase2BP->setEndValue(160.0);

    QParallelAnimationGroup *phase2 = new QParallelAnimationGroup();
    phase2->addAnimation(phase2Heart);
    phase2->addAnimation(phase2Amp);
    phase2->addAnimation(phase2SpO2);
    phase2->addAnimation(phase2BP);

            // 第三阶段：死亡状态 (10-16s视频段)
    QPropertyAnimation *phase3Heart = new QPropertyAnimation(this, "heartRate");
    phase3Heart->setDuration(6000);  // 6秒对应10-16s的视频段
    phase3Heart->setStartValue(180.0);
    phase3Heart->setEndValue(0.0);
    phase3Heart->setEasingCurve(QEasingCurve::OutCubic);

    QPropertyAnimation *phase3Amp = new QPropertyAnimation(this, "amplitude");
    phase3Amp->setDuration(6000);
    phase3Amp->setStartValue(1.6);
    phase3Amp->setEndValue(0.0);
    phase3Amp->setEasingCurve(QEasingCurve::OutQuad);

    QPropertyAnimation *phase3SpO2 = new QPropertyAnimation(this, "spO2");
    phase3SpO2->setDuration(6000);
    phase3SpO2->setStartValue(80.0);
    phase3SpO2->setEndValue(0.0);

    QPropertyAnimation *phase3BP = new QPropertyAnimation(this, "bloodPressure");
    phase3BP->setDuration(6000);
    phase3BP->setStartValue(160.0);
    phase3BP->setEndValue(0.0);

    QParallelAnimationGroup *phase3 = new QParallelAnimationGroup();
    phase3->addAnimation(phase3Heart);
    phase3->addAnimation(phase3Amp);
    phase3->addAnimation(phase3SpO2);
    phase3->addAnimation(phase3BP);

    m_mainAnimation->addAnimation(phase1);
    m_mainAnimation->addAnimation(phase2);
    m_mainAnimation->addAnimation(phase3);

            // 阶段变化监听
    connect(phase1, &QParallelAnimationGroup::stateChanged, this,
            [this](QAbstractAnimation::State newState, QAbstractAnimation::State){
                if (newState == QAbstractAnimation::Running) {
                    m_currentPhase = 1;
                    updateVideoForPhase();
                    updateHeartbeatTimer();
                }
            });

    connect(phase2, &QParallelAnimationGroup::stateChanged, this,
            [this](QAbstractAnimation::State newState, QAbstractAnimation::State){
                if (newState == QAbstractAnimation::Running) {
                    m_currentPhase = 2;
                    updateVideoForPhase();
                    updateHeartbeatTimer();
                }
            });

    connect(phase3, &QParallelAnimationGroup::stateChanged, this,
            [this](QAbstractAnimation::State newState, QAbstractAnimation::State){
                if (newState == QAbstractAnimation::Running) {
                    m_currentPhase = 3;
                    updateVideoForPhase();
                    updateHeartbeatTimer();
                }
            });

    connect(m_mainAnimation, &QSequentialAnimationGroup::finished, this, &ECGAnimation::animationFinished);
}

void ECGAnimation::updateVideoForPhase()
{
    if (!m_mediaPlayer) return;

    qint64 startTime, endTime;

    switch (m_currentPhase) {
    case 1: // 正常期间 (0-5s)
        startTime = m_normalStart;
        endTime = m_normalEnd;
        break;
    case 2: // 异常期间 (5-10s)
        startTime = m_abnormalStart;
        endTime = m_abnormalEnd;
        break;
    case 3: // 死亡期间 (10-16s)
        startTime = m_deathStart;
        endTime = m_deathEnd;
        break;
    default:
        startTime = m_normalStart;
        endTime = m_normalEnd;
        break;
    }

    // 设置播放位置到对应阶段的开始
    m_mediaPlayer->setPosition(startTime);
    m_mediaPlayer->play();
}

void ECGAnimation::onVideoPositionChanged(qint64 position)
{
    // 根据当前阶段检查是否需要循环播放
    qint64 endTime;
    qint64 startTime;

    switch (m_currentPhase) {
    case 1:
        startTime = m_normalStart;
        endTime = m_normalEnd;
        break;
    case 2:
        startTime = m_abnormalStart;
        endTime = m_abnormalEnd;
        break;
    case 3:
        startTime = m_deathStart;
        endTime = m_deathEnd;
        break;
    default:
        return;
    }

    // 如果播放位置超出当前阶段范围，重新开始
    if (position >= endTime || position < startTime) {
        m_mediaPlayer->setPosition(startTime);
    }
}

void ECGAnimation::startAnimation()
{
    m_displayTimer->start();
    updateHeartbeatTimer();
    m_heartbeatTimer->start();

    // 开始播放视频
    m_currentPhase = 1;
    updateVideoForPhase();

    // 开始参数动画
    m_mainAnimation->start();
}

void ECGAnimation::skipAnimation()
{
    m_mainAnimation->stop();
    m_displayTimer->stop();
    m_heartbeatTimer->stop();

    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
    }

    emit animationFinished();
}

void ECGAnimation::updateHeartbeatTimer()
{
    if (m_heartRate <= 0) {
        m_heartbeatTimer->stop();
        return;
    }
    int interval = qRound(60000.0 / m_heartRate);
    m_heartbeatTimer->setInterval(interval);
}

void ECGAnimation::triggerHeartbeat()
{
    if (m_heartRate <= 0) return;
    playHeartbeatSound();
}

void ECGAnimation::playHeartbeatSound()
{
   // 如果有音效管理器，取消注释以下代码
    /*
    switch (m_currentPhase) {
    case 1: MusicManager::instance()->playEffect("ecg_normal.mp3"); break;
    case 2: MusicManager::instance()->playEffect("ecg_warning.mp3"); break;
    case 3: if (m_heartRate > 0) { MusicManager::instance()->playEffect("ecg_death.mp3"); } break;
    }
    */
}

void ECGAnimation::updateDisplay()
{
    m_heartRateLabel->setText(QString("HR: %1").arg(qRound(m_heartRate)));
    m_spO2Label->setText(QString("SpO₂: %1%").arg(qRound(m_spO2)));
    m_bpLabel->setText(QString("BP: %1/80").arg(qRound(m_bloodPressure)));
    m_timeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    updateDisplayColors();

    if (m_heartRate <= 0 && !m_isFlatlined) {
        m_heartbeatTimer->stop();
        m_isFlatlined = true;
    }

    // 心率变化时更新定时器
    static double lastHeartRate = -1;
    if (qAbs(m_heartRate - lastHeartRate) > 1) {
        updateHeartbeatTimer();
        lastHeartRate = m_heartRate;
    }
}

void ECGAnimation::updateDisplayColors()
{
    QString normalStyle = "font-size: 24px; font-weight: bold; font-family: 'Courier New';";

    if (m_currentPhase == 1) {
        // 正常阶段 - 绿色
        m_heartRateLabel->setStyleSheet("color: #00FF00; " + normalStyle);
        m_spO2Label->setStyleSheet("color: #00FFFF; " + normalStyle);
        m_bpLabel->setStyleSheet("color: #FF8800; " + normalStyle);
    } else if (m_currentPhase == 2) {
        // 异常阶段 - 黄色闪烁
        static bool warningBlink = false;
        warningBlink = !warningBlink;
        QString color = warningBlink ? "yellow" : "orange";
        m_heartRateLabel->setStyleSheet("color: " + color + "; " + normalStyle);
        m_spO2Label->setStyleSheet("color: " + color + "; " + normalStyle);
        m_bpLabel->setStyleSheet("color: " + color + "; " + normalStyle);
    } else if (m_currentPhase == 3) {
        // 死亡阶段 - 红色闪烁
        if (m_heartRate <= 0) {
            static bool alarmBlink = false;
            alarmBlink = !alarmBlink;
            QString color = alarmBlink ? "red" : "darkred";
            m_heartRateLabel->setStyleSheet("color: " + color + "; " + normalStyle);
            m_spO2Label->setStyleSheet("color: " + color + "; " + normalStyle);
            m_bpLabel->setStyleSheet("color: " + color + "; " + normalStyle);
        } else {
            m_heartRateLabel->setStyleSheet("color: red; " + normalStyle);
            m_spO2Label->setStyleSheet("color: red; " + normalStyle);
            m_bpLabel->setStyleSheet("color: red; " + normalStyle);
        }
    }
}

void ECGAnimation::onPhaseChanged()
{
    updateVideoForPhase();
}

// 属性设置方法
void ECGAnimation::setAmplitude(qreal value)
{
    m_amplitude = value;
}

void ECGAnimation::setFrequency(qreal value)
{
    m_frequency = value;
}

void ECGAnimation::setHeartRate(qreal value)
{
    m_heartRate = value;
}

void ECGAnimation::setSpO2(qreal value)
{
    m_spO2 = value;
}

void ECGAnimation::setBloodPressure(qreal value)
{
    m_bloodPressure = value;
}
