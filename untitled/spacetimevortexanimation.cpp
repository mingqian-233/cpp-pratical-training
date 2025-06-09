// spacetimevortexanimation.cpp
#include "spacetimevortexanimation.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QRandomGenerator>
#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <QMouseEvent>
#include <QtMath>

SpaceTimeVortexAnimation::SpaceTimeVortexAnimation(QWidget *parent)
    : QWidget(parent),
      m_rotationSpeed(1.0),
      m_vortexDepth(1.0),
      m_colorIntensity(1.0),
      m_currentRotation(0.0)
{
    setupUI();
    setupAnimation();

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &SpaceTimeVortexAnimation::updateVortex);
    m_updateTimer->setInterval(16); // ~60fps

}

SpaceTimeVortexAnimation::~SpaceTimeVortexAnimation()
{
    m_updateTimer->stop();
    delete m_mainAnimation;

    // 清理图层对象
    for (auto item : m_vortexLayers) {
        delete item;
    }
    m_vortexLayers.clear();
}

void SpaceTimeVortexAnimation::setupUI()
{
    // 设置黑色背景
    setStyleSheet("background-color: black;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 创建图形视图
    m_view = new QGraphicsView(this);
    m_view->setStyleSheet("background-color: black; border: none;");
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_scene = new QGraphicsScene(this);
    m_view->setScene(m_scene);

    mainLayout->addWidget(m_view);

            // 添加文字标签
    setupTextLabel();

    // 跳过按钮
    QPushButton *skipButton = new QPushButton("跳过动画", this);
    skipButton->setStyleSheet("QPushButton { background-color: rgba(51, 51, 51, 150); color: white; "
        "border: 1px solid #555555; padding: 8px; border-radius: 4px; }"
        "QPushButton:hover { background-color: rgba(68, 68, 68, 180); }");
    connect(skipButton, &QPushButton::clicked, this, &SpaceTimeVortexAnimation::skipAnimation);

    // 放在右下角
    skipButton->setFixedSize(100, 40);
    skipButton->move(this->width() - skipButton->width() - 20, this->height() - skipButton->height() - 20);

    // 预先创建漩涡层
    for (int i = 0; i < 12; i++) {
        QGraphicsPathItem *layer = new QGraphicsPathItem();
        QColor layerColor = generateRandomColor(m_colorIntensity);
        QPen pen(layerColor, 2 + i);
        pen.setCapStyle(Qt::RoundCap);
        layer->setPen(pen);

        // 添加发光效果
        QGraphicsDropShadowEffect *glowEffect = new QGraphicsDropShadowEffect();
        glowEffect->setBlurRadius(10);
        glowEffect->setColor(layerColor);
        glowEffect->setOffset(0, 0);
        layer->setGraphicsEffect(glowEffect);

        m_scene->addItem(layer);
        m_vortexLayers.append(layer);
    }
}
void SpaceTimeVortexAnimation::setupTextLabel()
{
    // 创建两个文字标签
    m_textLabel1 = new QLabel(this);
    m_textLabel1->setText("我的身体越来越重，某个部分却越来越轻。");

    m_textLabel2 = new QLabel(this);
    m_textLabel2->setText("一股强大的吸力传来，好似要把我的魂魄吸入未知处……");

            // 设置楷体大字
    QFont font("楷体", 28, QFont::Bold);
    m_textLabel1->setFont(font);
    m_textLabel2->setFont(font);

            // 设置样式
    m_textLabel1->setAlignment(Qt::AlignCenter);
    m_textLabel1->setStyleSheet("color: white; background-color: transparent;");

    m_textLabel2->setAlignment(Qt::AlignCenter);
    m_textLabel2->setStyleSheet("color: white; background-color: transparent;");

            // 添加描边效果，使文字在漩涡背景上更清晰
    QGraphicsDropShadowEffect *textShadow1 = new QGraphicsDropShadowEffect(this);
    textShadow1->setBlurRadius(10);
    textShadow1->setColor(Qt::black);
    textShadow1->setOffset(0, 0);
    m_textLabel1->setGraphicsEffect(textShadow1);

    QGraphicsDropShadowEffect *textShadow2 = new QGraphicsDropShadowEffect(this);
    textShadow2->setBlurRadius(10);
    textShadow2->setColor(Qt::black);
    textShadow2->setOffset(0, 0);
    m_textLabel2->setGraphicsEffect(textShadow2);

            // 初始设为透明
    m_textLabel1->setWindowOpacity(0.0);
    m_textLabel2->setWindowOpacity(0.0);
    m_textLabel2->hide(); // 第二个文本初始隐藏

            // 创建文本动画序列
    m_textAnimationGroup = new QSequentialAnimationGroup(this);

    // 第一段文字淡入动画
    QPropertyAnimation *fadeIn1 = new QPropertyAnimation(m_textLabel1, "windowOpacity");
    fadeIn1->setDuration(1500);
    fadeIn1->setStartValue(0.0);
    fadeIn1->setEndValue(1.0);
    fadeIn1->setEasingCurve(QEasingCurve::InOutQuad);

    // 第一段文字停留
    QPropertyAnimation *stay1 = new QPropertyAnimation(m_textLabel1, "windowOpacity");
    stay1->setDuration(2000);
    stay1->setStartValue(1.0);
    stay1->setEndValue(1.0);

    // 第一段文字淡出
    QPropertyAnimation *fadeOut1 = new QPropertyAnimation(m_textLabel1, "windowOpacity");
    fadeOut1->setDuration(1500);
    fadeOut1->setStartValue(1.0);
    fadeOut1->setEndValue(0.0);
    fadeOut1->setEasingCurve(QEasingCurve::InOutQuad);

    // 第二段文字淡入
    QPropertyAnimation *fadeIn2 = new QPropertyAnimation(m_textLabel2, "windowOpacity");
    fadeIn2->setDuration(1500);
    fadeIn2->setStartValue(0.0);
    fadeIn2->setEndValue(1.0);
    fadeIn2->setEasingCurve(QEasingCurve::InOutQuad);

    // 添加到动画序列
    m_textAnimationGroup->addAnimation(fadeIn1);
    m_textAnimationGroup->addAnimation(stay1);
    m_textAnimationGroup->addAnimation(fadeOut1);
    m_textAnimationGroup->addAnimation(fadeIn2);

    // 连接动画过渡信号
    connect(fadeOut1, &QPropertyAnimation::finished, [this]() {
        m_textLabel1->hide();
        m_textLabel2->show();
    });
}

void SpaceTimeVortexAnimation::setupAnimation()
{
    m_mainAnimation = new QSequentialAnimationGroup(this);

    // 第一阶段：漩涡形成和加速
    QPropertyAnimation *phase1Rotation = new QPropertyAnimation(this, "rotationSpeed");
    phase1Rotation->setDuration(3000);
    phase1Rotation->setStartValue(0.2);
    phase1Rotation->setEndValue(2.0);
    phase1Rotation->setEasingCurve(QEasingCurve::InSine);

    QPropertyAnimation *phase1Depth = new QPropertyAnimation(this, "vortexDepth");
    phase1Depth->setDuration(3000);
    phase1Depth->setStartValue(0.2);
    phase1Depth->setEndValue(1.0);
    phase1Depth->setEasingCurve(QEasingCurve::InOutQuad);

    QPropertyAnimation *phase1Color = new QPropertyAnimation(this, "colorIntensity");
    phase1Color->setDuration(3000);
    phase1Color->setStartValue(0.3);
    phase1Color->setEndValue(1.0);

    QParallelAnimationGroup *phase1 = new QParallelAnimationGroup();
    phase1->addAnimation(phase1Rotation);
    phase1->addAnimation(phase1Depth);
    phase1->addAnimation(phase1Color);

    // 第二阶段：漩涡剧烈变化
    QPropertyAnimation *phase2Rotation = new QPropertyAnimation(this, "rotationSpeed");
    phase2Rotation->setDuration(4000);
    phase2Rotation->setStartValue(2.0);
    phase2Rotation->setEndValue(4.0);
    phase2Rotation->setEasingCurve(QEasingCurve::InOutBack);

    QPropertyAnimation *phase2Depth = new QPropertyAnimation(this, "vortexDepth");
    phase2Depth->setDuration(4000);
    phase2Depth->setStartValue(1.0);
    phase2Depth->setEndValue(2.0);
    phase2Depth->setEasingCurve(QEasingCurve::OutInQuad);

    QPropertyAnimation *phase2Color = new QPropertyAnimation(this, "colorIntensity");
    phase2Color->setDuration(4000);
    phase2Color->setStartValue(1.0);
    phase2Color->setEndValue(1.5);

    QParallelAnimationGroup *phase2 = new QParallelAnimationGroup();
    phase2->addAnimation(phase2Rotation);
    phase2->addAnimation(phase2Depth);
    phase2->addAnimation(phase2Color);

    // 第三阶段：穿越速度加快，光效强烈
    QPropertyAnimation *phase3Rotation = new QPropertyAnimation(this, "rotationSpeed");
    phase3Rotation->setDuration(3000);
    phase3Rotation->setStartValue(4.0);
    phase3Rotation->setEndValue(6.0);
    phase3Rotation->setEasingCurve(QEasingCurve::InQuad);

    QPropertyAnimation *phase3Depth = new QPropertyAnimation(this, "vortexDepth");
    phase3Depth->setDuration(3000);
    phase3Depth->setStartValue(2.0);
    phase3Depth->setEndValue(3.0);
    phase3Depth->setEasingCurve(QEasingCurve::InExpo);

    QPropertyAnimation *phase3Color = new QPropertyAnimation(this, "colorIntensity");
    phase3Color->setDuration(3000);
    phase3Color->setStartValue(1.5);
    phase3Color->setEndValue(2.0);

    QParallelAnimationGroup *phase3 = new QParallelAnimationGroup();
    phase3->addAnimation(phase3Rotation);
    phase3->addAnimation(phase3Depth);
    phase3->addAnimation(phase3Color);

    // 添加到主动画
    m_mainAnimation->addAnimation(phase1);
    m_mainAnimation->addAnimation(phase2);
    m_mainAnimation->addAnimation(phase3);

    connect(m_mainAnimation, &QSequentialAnimationGroup::finished, this, &SpaceTimeVortexAnimation::animationFinished);
}

void SpaceTimeVortexAnimation::startAnimation()
{
    m_updateTimer->start();
    m_mainAnimation->start();

    // 调整文字大小和位置
    int textWidth = width() * 0.8;
    int textHeight = height() / 4;
    int textX = (width() - textWidth) / 2;
    int textY = (height() - textHeight) / 2;

    m_textLabel1->resize(textWidth, textHeight);
    m_textLabel1->move(textX, textY);

    m_textLabel2->resize(textWidth, textHeight);
    m_textLabel2->move(textX, textY);

            // 显示第一段文字并启动文字动画
    m_textLabel1->show();
    m_textLabel1->raise();  // 确保文字在最上层
    m_textLabel2->raise();  // 确保文字在最上层
    m_textAnimationGroup->start();
}

void SpaceTimeVortexAnimation::skipAnimation()
{
    m_mainAnimation->stop();
    m_updateTimer->stop();
    emit animationFinished();
}

void SpaceTimeVortexAnimation::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 调整场景大小
    if (m_scene && m_view) {
        m_view->resize(size());
        m_scene->setSceneRect(0, 0, width(), height());

        // 重绘漩涡
        drawVortex();
    }

    // 重新定位跳过按钮
    QList<QPushButton*> buttons = findChildren<QPushButton*>();
    if (!buttons.isEmpty()) {
        QPushButton *skipButton = buttons.first();
        skipButton->move(this->width() - skipButton->width() - 20, this->height() - skipButton->height() - 20);
    }
}

void SpaceTimeVortexAnimation::mousePressEvent(QMouseEvent *event)
{
    // 点击任意位置跳过动画
    event->accept();
}

void SpaceTimeVortexAnimation::updateVortex()
{
    // 更新旋转角度
    m_currentRotation += m_rotationSpeed;
    if (m_currentRotation >= 360.0) {
        m_currentRotation -= 360.0;
    }

    // 重绘漩涡
    drawVortex();
}

void SpaceTimeVortexAnimation::drawVortex()
{
    int centerX = width() / 2;
    int centerY = height() / 2;
    qreal maxRadius = qMin(width(), height()) * 0.9 / 2;

    // 为每一层更新路径
    for (int i = 0; i < m_vortexLayers.size(); i++) {
        QGraphicsPathItem *layer = m_vortexLayers[i];

        // 计算这一层的参数
        qreal depthFactor = 1.0 - (i / (qreal)m_vortexLayers.size()) * m_vortexDepth;
        qreal layerRadius = maxRadius * depthFactor;

        // 创建螺旋路径
        QPainterPath path;
        path.moveTo(centerX, centerY);

        qreal angleOffset = m_currentRotation + i * 30.0; // 每层有不同的起始角度

        for (qreal angle = 0; angle <= 1080.0; angle += 10.0) { // 三圈螺旋
            qreal currentAngle = angle + angleOffset;
            qreal spiralRadius = layerRadius * (angle / 1080.0); // 从中心向外扩展

            qreal x = centerX + spiralRadius * qCos(qDegreesToRadians(currentAngle));
            qreal y = centerY + spiralRadius * qSin(qDegreesToRadians(currentAngle));

            path.lineTo(x, y);
        }

        layer->setPath(path);

        // 更新颜色
        QColor layerColor = generateRandomColor(m_colorIntensity);
        QPen pen = layer->pen();
        pen.setColor(layerColor);
        layer->setPen(pen);

        // 更新发光效果
        QGraphicsDropShadowEffect *effect = static_cast<QGraphicsDropShadowEffect*>(layer->graphicsEffect());
        if (effect) {
            effect->setColor(layerColor);
        }
    }
}

QColor SpaceTimeVortexAnimation::generateRandomColor(qreal intensity)
{
    // 生成鲜艳的随机颜色
    QRandomGenerator *rng = QRandomGenerator::global();

    // 基础色相 - 更倾向于蓝紫色调以营造神秘感
    int hue = rng->bounded(150, 300); // 蓝色到紫色范围

    // 低饱和度和亮度
    int saturation = rng->bounded(100, 150);
    int value = rng->bounded(100, 150);

    // 应用强度因子
    saturation = qMin(255, qRound(saturation * intensity));
    value = qMin(255, qRound(value * intensity));

    return QColor::fromHsv(hue, saturation, value);
}

void SpaceTimeVortexAnimation::setRotationSpeed(qreal value)
{
    m_rotationSpeed = value;
}

void SpaceTimeVortexAnimation::setVortexDepth(qreal value)
{
    m_vortexDepth = value;
}

void SpaceTimeVortexAnimation::setColorIntensity(qreal value)
{
    m_colorIntensity = value;
}
