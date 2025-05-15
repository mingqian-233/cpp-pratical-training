// ecganimation.cpp
#include "ecganimation.h"
#include <QPainter>
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QtMath>
#include <QRandomGenerator>
#include <QPushButton>
#include "musicmanager.h"

ECGAnimation::ECGAnimation(QWidget *parent)
    : QWidget(parent),
      m_amplitude(1.0),
      m_frequency(1.0),
      m_heartRate(80.0),
      m_spO2(98.0),
      m_bloodPressure(120.0),
      m_gridSize(10),
      m_currentPhase(0)
{
    setupUI();
    setupAnimation();

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &ECGAnimation::updateWaveform);
    m_updateTimer->setInterval(50);

    m_displayTimer = new QTimer(this);
    connect(m_displayTimer, &QTimer::timeout, this, &ECGAnimation::updateDisplay);
    m_displayTimer->setInterval(1000);

    // 心跳音效计时器
    m_heartbeatTimer = new QTimer(this);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &ECGAnimation::playHeartbeatSound);

}

ECGAnimation::~ECGAnimation()
{
    m_updateTimer->stop();
    m_displayTimer->stop();
    delete m_mainAnimation;
}

void ECGAnimation::setupUI()
{
    // 设置黑色背景
    setStyleSheet("background-color: black;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 创建顶部信息面板
    QHBoxLayout *infoLayout = new QHBoxLayout();

    m_heartRateLabel = new QLabel("HR: 80", this);
    m_heartRateLabel->setStyleSheet("color: #00FF00; font-size: 24px; font-weight: bold;");

    m_spO2Label = new QLabel("SpO₂: 98%", this);
    m_spO2Label->setStyleSheet("color: #00FFFF; font-size: 24px; font-weight: bold;");

    m_bpLabel = new QLabel("BP: 120/80", this);
    m_bpLabel->setStyleSheet("color: #FF8800; font-size: 24px; font-weight: bold;");

    m_timeLabel = new QLabel(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), this);
    m_timeLabel->setStyleSheet("color: white; font-size: 24px;");

    infoLayout->addWidget(m_heartRateLabel);
    infoLayout->addWidget(m_spO2Label);
    infoLayout->addWidget(m_bpLabel);
    infoLayout->addStretch();
    infoLayout->addWidget(m_timeLabel);

    mainLayout->addLayout(infoLayout);

    // 创建心电图视图
    m_view = new QGraphicsView(this);
    m_view->setStyleSheet("background-color: black; border: 1px solid #333333;");
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_scene = new QGraphicsScene(this);
    m_view->setScene(m_scene);

    // 添加网格
    QPen gridPen(QColor(0, 80, 0, 100));
    gridPen.setWidth(1);

    int width = 800;
    int height = 400;

    for (int x = 0; x <= width; x += m_gridSize) {
        m_scene->addLine(x, 0, x, height, gridPen);
    }

    for (int y = 0; y <= height; y += m_gridSize) {
        m_scene->addLine(0, y, width, y, gridPen);
    }

    // 每隔5个小格加粗
    QPen mainGridPen(QColor(0, 120, 0, 150));
    mainGridPen.setWidth(2);

    for (int x = 0; x <= width; x += m_gridSize * 5) {
        m_scene->addLine(x, 0, x, height, mainGridPen);
    }

    for (int y = 0; y <= height; y += m_gridSize * 5) {
        m_scene->addLine(0, y, width, y, mainGridPen);
    }

    // 创建ECG路径
    QPen ecgPen(QColor(0, 255, 0));
    ecgPen.setWidth(3);

    m_ecgPath = new QGraphicsPathItem();
    m_ecgPath->setPen(ecgPen);
    m_scene->addItem(m_ecgPath);

    m_view->setFixedSize(width + 2, height + 2);
    m_scene->setSceneRect(0, 0, width, height);

    mainLayout->addWidget(m_view);
    mainLayout->setAlignment(m_view, Qt::AlignCenter);

    // 底部信息
    QLabel *deviceInfoLabel = new QLabel("医疗监护系统 v2.0 | 重症监护室 | 病床号: 713", this);
    deviceInfoLabel->setStyleSheet("color: #888888; font-size: 18px;");
    deviceInfoLabel->setAlignment(Qt::AlignRight);

    mainLayout->addWidget(deviceInfoLabel);

    // 跳过按钮
    QPushButton *skipButton = new QPushButton("跳过动画", this);
    skipButton->setStyleSheet("QPushButton { background-color: #333333; color: white; "
        "border: 1px solid #555555; padding: 8px; border-radius: 4px; }"
        "QPushButton:hover { background-color: #444444; }");
    connect(skipButton, &QPushButton::clicked, this, &ECGAnimation::skipAnimation);

    mainLayout->addWidget(skipButton, 0, Qt::AlignRight);
}

void ECGAnimation::setupAnimation()
{
    m_mainAnimation = new QSequentialAnimationGroup(this);

            // 第一阶段：正常心跳，逐步加快
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

            // 第二阶段：警告状态，心跳更快，波形更不规则
    QPropertyAnimation *phase2Heart = new QPropertyAnimation(this, "heartRate");
    phase2Heart->setDuration(3000);
    phase2Heart->setStartValue(120.0);
    phase2Heart->setEndValue(180.0);
    phase2Heart->setEasingCurve(QEasingCurve::InCubic);

    QPropertyAnimation *phase2Amp = new QPropertyAnimation(this, "amplitude");
    phase2Amp->setDuration(3000);
    phase2Amp->setStartValue(1.2);
    phase2Amp->setEndValue(1.6);
    phase2Amp->setEasingCurve(QEasingCurve::InOutQuad);

    QPropertyAnimation *phase2SpO2 = new QPropertyAnimation(this, "spO2");
    phase2SpO2->setDuration(3000);
    phase2SpO2->setStartValue(95.0);
    phase2SpO2->setEndValue(80.0);

    QPropertyAnimation *phase2BP = new QPropertyAnimation(this, "bloodPressure");
    phase2BP->setDuration(3000);
    phase2BP->setStartValue(130.0);
    phase2BP->setEndValue(160.0);

    QParallelAnimationGroup *phase2 = new QParallelAnimationGroup();
    phase2->addAnimation(phase2Heart);
    phase2->addAnimation(phase2Amp);
    phase2->addAnimation(phase2SpO2);
    phase2->addAnimation(phase2BP);

            // 第三阶段：死亡
    QPropertyAnimation *phase3Heart = new QPropertyAnimation(this, "heartRate");
    phase3Heart->setDuration(5000);
    phase3Heart->setStartValue(180.0);
    phase3Heart->setEndValue(0.0);
    phase3Heart->setEasingCurve(QEasingCurve::OutCubic);

    QPropertyAnimation *phase3Amp = new QPropertyAnimation(this, "amplitude");
    phase3Amp->setDuration(5000);
    phase3Amp->setStartValue(1.6);
    phase3Amp->setEndValue(0.0);
    phase3Amp->setEasingCurve(QEasingCurve::OutQuad);

    QPropertyAnimation *phase3SpO2 = new QPropertyAnimation(this, "spO2");
    phase3SpO2->setDuration(5000);
    phase3SpO2->setStartValue(80.0);
    phase3SpO2->setEndValue(0.0);

    QPropertyAnimation *phase3BP = new QPropertyAnimation(this, "bloodPressure");
    phase3BP->setDuration(5000);
    phase3BP->setStartValue(160.0);
    phase3BP->setEndValue(0.0);

    QParallelAnimationGroup *phase3 = new QParallelAnimationGroup();
    phase3->addAnimation(phase3Heart);
    phase3->addAnimation(phase3Amp);
    phase3->addAnimation(phase3SpO2);
    phase3->addAnimation(phase3BP);

            // 添加到主动画
    m_mainAnimation->addAnimation(phase1);
    m_mainAnimation->addAnimation(phase2);
    m_mainAnimation->addAnimation(phase3);

            // 监听各个阶段的变化 - 修复错误，使用stateChanged信号
    connect(phase1, &QParallelAnimationGroup::stateChanged, this, [this](QAbstractAnimation::State newState, QAbstractAnimation::State oldState){
        if (newState == QAbstractAnimation::Running && oldState != QAbstractAnimation::Running) {
            m_currentPhase = 1;
            MusicManager::instance()->playEffect("qrc:/music/ecg_normal.mp3");

            // 根据心率设置心跳音效的间隔
            updateHeartbeatTimer();
        }
    });

    connect(phase2, &QParallelAnimationGroup::stateChanged, this, [this](QAbstractAnimation::State newState, QAbstractAnimation::State oldState){
        if (newState == QAbstractAnimation::Running && oldState != QAbstractAnimation::Running) {
            m_currentPhase = 2;
            MusicManager::instance()->playEffect("qrc:/music/ecg_warning.mp3");

            // 更新心跳音效的间隔
            updateHeartbeatTimer();
        }
    });

    connect(phase3, &QParallelAnimationGroup::stateChanged, this, [this](QAbstractAnimation::State newState, QAbstractAnimation::State oldState){
        if (newState == QAbstractAnimation::Running && oldState != QAbstractAnimation::Running) {
            m_currentPhase = 3;
            MusicManager::instance()->playEffect("ecg_death.mp3");

            // 停止心跳音效
            m_heartbeatTimer->stop();
        }
    });

    connect(m_mainAnimation, &QSequentialAnimationGroup::finished, this, &ECGAnimation::animationFinished);
}


void ECGAnimation::startAnimation()
{
    m_updateTimer->start();
    m_displayTimer->start();
    m_mainAnimation->start();

    // 开始心跳声音计时器
    updateHeartbeatTimer();
    m_heartbeatTimer->start();
}

void ECGAnimation::skipAnimation()
{
    m_mainAnimation->stop();
    m_updateTimer->stop();
    m_displayTimer->stop();
    m_heartbeatTimer->stop();
    emit animationFinished();
}

void ECGAnimation::updateHeartbeatTimer()
{
    if (m_heartRate <= 0) {
        m_heartbeatTimer->stop();
        return;
    }

    // 根据心率计算两次心跳之间的间隔(毫秒)
    int interval = qRound(60.0 / m_heartRate * 1000);
    m_heartbeatTimer->setInterval(interval);
}
void ECGAnimation::playHeartbeatSound()
{
    // 根据当前阶段播放不同的心跳音效
    switch (m_currentPhase) {
    case 1:
        MusicManager::instance()->playEffect("ecg_normal.mp3");
        break;
    case 2:
        MusicManager::instance()->playEffect("ecg_warning.mp3");
        break;
    case 3:
        break;
    }
}

void ECGAnimation::updateWaveform()
{
    // 根据当前振幅和频率生成ECG路径
    QPainterPath path = generateECGPath(m_amplitude, m_frequency);
    m_ecgPath->setPath(path);
}

QPainterPath ECGAnimation::generateECGPath(qreal amplitude, qreal frequency)
{
    QPainterPath path;

            // 心电图视图的中心线
    int centerY = m_scene->height() / 2;
    int width = m_scene->width();

            // 基础ECG波形参数
    qreal p_width = 20 * frequency;
    qreal p_height = 10 * amplitude;
    qreal q_depth = 5 * amplitude;
    qreal qrs_width = 10 * frequency;
    qreal r_height = 80 * amplitude;
    qreal s_depth = 20 * amplitude;
    qreal t_width = 25 * frequency;
    qreal t_height = 20 * amplitude;

            // 添加一些随机变化，使波形更自然
    QRandomGenerator *rand = QRandomGenerator::global();

    // 在第二阶段增加更多的波动，表示心律不齐
    qreal noiseAmplitude = 2.0;
    if (m_currentPhase == 2) {
        noiseAmplitude = 5.0;

        // 在警告阶段偶尔添加早搏或漏搏
        if (rand->bounded(100) < 20) {  // 20%的概率出现异常
            p_height += (rand->bounded(1000) / 100.0) - 5.0;
            r_height += (rand->bounded(2000) / 100.0) - 10.0;
        }
    } else {
        p_height += (rand->bounded(400) / 100.0) - 2.0;  // -2.0到2.0之间
        r_height += (rand->bounded(1000) / 100.0) - 5.0; // -5.0到5.0之间
    }

    t_height += (rand->bounded(600) / 100.0) - 3.0;  // -3.0到3.0之间

            // 如果是濒死状态（振幅接近0），则生成几乎是直线的波形
    if (amplitude < 0.1) {
        path.moveTo(0, centerY);

        for (int x = 0; x < width; x += 2) {
            qreal y = centerY + (rand->bounded(400) / 100.0) - 2.0;
            path.lineTo(QPointF(x, y));
        }

        return path;
    }

            // 计算一个完整心跳的宽度
    int beatWidth = qRound(p_width + qrs_width + t_width + 50);

            // 根据心率计算两次心跳之间的间隔
    qreal interval = 60.0 / qMax(1.0, m_heartRate) * 1000;  // 60秒/每分钟心跳数 * 1000毫秒
    interval = qMax(interval, (qreal)beatWidth); // 确保间隔至少足够一个心跳

            // 转换为像素单位
    interval = interval / 10.0;

            // 基于时间创建波形
    QDateTime now = QDateTime::currentDateTime();
    qint64 msecs = now.toMSecsSinceEpoch();

    path.moveTo(0, centerY);

            // 先添加一些基线
    int x = 0;
    while (x < 30) {
        qreal y = centerY + (rand->bounded(200) / 100.0) - 1.0;
        path.lineTo(QPointF(x, y));
        x += 2;
    }

            // 计算将会有多少个心跳周期
    int numBeats = qCeil(width / interval) + 1;

    for (int beat = 0; beat < numBeats; beat++) {
        int beatStart = qRound(beat * interval);

                // P波
        for (int i = 0; i < p_width; i++) {
            if (x >= width) break;
            qreal ratio = (qreal)i / p_width;
            qreal y = centerY - p_height * qSin(ratio * M_PI);

            // 在第2阶段添加更多噪声
            if (m_currentPhase == 2) {
                y += (rand->bounded(400) / 100.0) - 2.0;
            }

            path.lineTo(QPointF(x, y));
            x += 1;
        }

                // 基线
        for (int i = 0; i < 10; i++) {
            if (x >= width) break;
            qreal baselineNoise = (rand->bounded(200) / 100.0) - 1.0;
            path.lineTo(QPointF(x, centerY + baselineNoise));
            x += 1;
        }

                // QRS波
                // Q波
        if (x < width) {
            qreal qNoise = 0;
            if (m_currentPhase == 2) qNoise = (rand->bounded(300) / 100.0) - 1.5;
            path.lineTo(QPointF(x, centerY + q_depth + qNoise));
            x += 2;
        }

                // R波
        if (x < width) {
            qreal rNoise = 0;
            if (m_currentPhase == 2) rNoise = (rand->bounded(1000) / 100.0) - 5.0;
            path.lineTo(QPointF(x, centerY - r_height + rNoise));
            x += 3;
        }

                // S波
        if (x < width) {
            qreal sNoise = 0;
            if (m_currentPhase == 2) sNoise = (rand->bounded(600) / 100.0) - 3.0;
            path.lineTo(QPointF(x, centerY + s_depth + sNoise));
            x += 2;
        }

                // 回到基线
        if (x < width) {
            path.lineTo(QPointF(x, centerY));
            x += 3;
        }

                // T波
        for (int i = 0; i < t_width; i++) {
            if (x >= width) break;
            qreal ratio = (qreal)i / t_width;
            qreal y = centerY - t_height * qSin(ratio * M_PI);

            // 在第2阶段添加更多噪声
            if (m_currentPhase == 2) {
                y += (rand->bounded(400) / 100.0) - 2.0;
            }

            path.lineTo(QPointF(x, y));
            x += 1;
        }

                // 剩余基线到下一个心跳周期
        int remaining = beatStart + qRound(interval) - x;
        for (int i = 0; i < remaining; i++) {
            if (x >= width) break;
            qreal y = centerY + (rand->bounded(200) / 100.0) - 1.0;

            // 第2阶段增加基线漂移
            if (m_currentPhase == 2) {
                y += (rand->bounded(200) / 100.0) - 1.0;
            }

            path.lineTo(QPointF(x, y));
            x += 2;
        }
    }

    return path;
}
void ECGAnimation::updateDisplay()
{
    // 更新显示的值
    m_heartRateLabel->setText(QString("HR: %1").arg(qRound(m_heartRate)));
    m_spO2Label->setText(QString("SpO₂: %1%").arg(qRound(m_spO2)));
    m_bpLabel->setText(QString("BP: %1/80").arg(qRound(m_bloodPressure)));
    m_timeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

            // 根据当前阶段更新颜色
    if (m_currentPhase == 1) {
        // 正常阶段
        if (m_heartRate > 100) {
            m_heartRateLabel->setStyleSheet("color: #00FF00; font-size: 24px; font-weight: bold;");
        } else {
            m_heartRateLabel->setStyleSheet("color: #00FF00; font-size: 24px; font-weight: bold;");
        }
        m_spO2Label->setStyleSheet("color: #00FFFF; font-size: 24px; font-weight: bold;");
        m_bpLabel->setStyleSheet("color: #FF8800; font-size: 24px; font-weight: bold;");
    } else if (m_currentPhase == 2) {
        // 警告阶段
        m_heartRateLabel->setStyleSheet("color: yellow; font-size: 24px; font-weight: bold;");
        m_spO2Label->setStyleSheet("color: yellow; font-size: 24px; font-weight: bold;");
        m_bpLabel->setStyleSheet("color: yellow; font-size: 24px; font-weight: bold;");

        // 在警告阶段使数字闪烁
        static bool blink = false;
        blink = !blink;

        if (blink && m_heartRate > 150) {
            m_heartRateLabel->setStyleSheet("color: red; font-size: 24px; font-weight: bold;");
        }
    } else if (m_currentPhase == 3) {
        // 死亡阶段
        if (m_heartRate < 20) {
            m_heartRateLabel->setStyleSheet("color: red; font-size: 24px; font-weight: bold;");
            m_spO2Label->setStyleSheet("color: red; font-size: 24px; font-weight: bold;");
            m_bpLabel->setStyleSheet("color: red; font-size: 24px; font-weight: bold;");
        } else {
            m_heartRateLabel->setStyleSheet("color: orange; font-size: 24px; font-weight: bold;");
            m_spO2Label->setStyleSheet("color: orange; font-size: 24px; font-weight: bold;");
            m_bpLabel->setStyleSheet("color: orange; font-size: 24px; font-weight: bold;");
        }
    }

            // 心跳为0时播放报警
    if (m_heartRate <= 0) {
        // 所有读数闪烁
        static bool alarmBlink = false;
        alarmBlink = !alarmBlink;

        if (alarmBlink) {
            m_heartRateLabel->setStyleSheet("color: red; font-size: 24px; font-weight: bold;");
            m_spO2Label->setStyleSheet("color: red; font-size: 24px; font-weight: bold;");
            m_bpLabel->setStyleSheet("color: red; font-size: 24px; font-weight: bold;");
        } else {
            m_heartRateLabel->setStyleSheet("color: black; font-size: 24px; font-weight: bold;");
            m_spO2Label->setStyleSheet("color: black; font-size: 24px; font-weight: bold;");
            m_bpLabel->setStyleSheet("color: black; font-size: 24px; font-weight: bold;");
        }
    }

    // 如果心率有变化，更新心跳声音计时器
    static int lastHeartRate = -1;
    if (qAbs(m_heartRate - lastHeartRate) > 1) {
        updateHeartbeatTimer();
        lastHeartRate = m_heartRate;
    }
}

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
