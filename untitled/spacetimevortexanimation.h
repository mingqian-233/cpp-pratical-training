// spacetimevortexanimation.h
#ifndef SPACETIMEVORTEXANIMATION_H
#define SPACETIMEVORTEXANIMATION_H

#include <QWidget>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsBlurEffect>
#include <QList>
#include <QPainterPath>
#include <QLabel>
class SpaceTimeVortexAnimation : public QWidget
{
    Q_OBJECT

    // 属性定义，用于动画
    Q_PROPERTY(qreal rotationSpeed READ rotationSpeed WRITE setRotationSpeed)
    Q_PROPERTY(qreal vortexDepth READ vortexDepth WRITE setVortexDepth)
    Q_PROPERTY(qreal colorIntensity READ colorIntensity WRITE setColorIntensity)

public:
    explicit SpaceTimeVortexAnimation(QWidget *parent = nullptr);
    ~SpaceTimeVortexAnimation();

    void startAnimation();
    void skipAnimation();

    // 属性获取函数
    qreal rotationSpeed() const { return m_rotationSpeed; }
    qreal vortexDepth() const { return m_vortexDepth; }
    qreal colorIntensity() const { return m_colorIntensity; }

    // 属性设置函数
    void setRotationSpeed(qreal value);
    void setVortexDepth(qreal value);
    void setColorIntensity(qreal value);

signals:
    void animationFinished();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void updateVortex();

private:
    void setupUI();
    void setupAnimation();
    void drawVortex();
    QColor generateRandomColor(qreal intensity);
    void setupTextLabel();  // 新增函数

    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    QTimer *m_updateTimer;
    QPropertyAnimation *m_rotationAnimation;
    QPropertyAnimation *m_depthAnimation;
    QPropertyAnimation *m_colorAnimation;
    QSequentialAnimationGroup *m_mainAnimation;

    QList<QGraphicsPathItem*> m_vortexLayers;

    // 动画属性
    qreal m_rotationSpeed;
    qreal m_vortexDepth;
    qreal m_colorIntensity;

    // 旋转角度
    qreal m_currentRotation;
private:
    // 替换原来的单个文本标签和动画
    QLabel *m_textLabel1;
    QLabel *m_textLabel2;
    QSequentialAnimationGroup *m_textAnimationGroup;

};

#endif // SPACETIMEVORTEXANIMATION_H
