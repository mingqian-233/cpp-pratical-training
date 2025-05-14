// ChapterTransition.cpp
#include "ChapterTransition.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QFont>
#include <QFontDatabase>

ChapterTransition::ChapterTransition(QWidget *parent)
    : QWidget(parent)
      , m_opacity(0.0)
{
    // 设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background-color: transparent;");

    // 加载背景图片
    m_background = QPixmap(":/images/chapter.png");

    // 创建标题标签
    m_titleLabel = new QLabel(this);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    // 加载楷体或设置字体
    int fontId = QFontDatabase::addApplicationFont(":/fonts/kaiti.ttf");
    QString fontFamily;
    if (fontId != -1) {
        fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    } else {
        fontFamily = "楷体"; // 回退到系统楷体
    }

    QFont chapterFont(fontFamily, 48);
    chapterFont.setBold(true);
    m_titleLabel->setFont(chapterFont);
    m_titleLabel->setStyleSheet("color: white; background-color: transparent;");

    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch(1);
    layout->addWidget(m_titleLabel);
    layout->addStretch(1);

    // 创建动画组
    m_animationGroup = new QSequentialAnimationGroup(this);

    // 淡入动画 (2秒)
    m_fadeInAnimation = new QPropertyAnimation(this, "opacity");
    m_fadeInAnimation->setDuration(2000);
    m_fadeInAnimation->setStartValue(0.0);
    m_fadeInAnimation->setEndValue(1.0);
    m_fadeInAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // 保持显示动画 (2秒)
    m_holdAnimation = new QPropertyAnimation(this, "opacity");
    m_holdAnimation->setDuration(2000);
    m_holdAnimation->setStartValue(1.0);
    m_holdAnimation->setEndValue(1.0);

    // 淡出动画 (1.5秒)
    m_fadeOutAnimation = new QPropertyAnimation(this, "opacity");
    m_fadeOutAnimation->setDuration(1500);
    m_fadeOutAnimation->setStartValue(1.0);
    m_fadeOutAnimation->setEndValue(0.0);
    m_fadeOutAnimation->setEasingCurve(QEasingCurve::InCubic);

    // 添加到动画组
    m_animationGroup->addAnimation(m_fadeInAnimation);
    m_animationGroup->addAnimation(m_holdAnimation);
    m_animationGroup->addAnimation(m_fadeOutAnimation);

    // 连接动画完成信号
    connect(m_animationGroup, &QSequentialAnimationGroup::finished, [this](){
        this->hide();
        emit transitionFinished();
    });

}


void ChapterTransition::showChapter(const QString &chapterTitle)
{
    // 设置章节标题
    m_titleLabel->setText(chapterTitle);

    // 显示自己
    show();
    raise();
    setFocus();

    // 重置并开始动画
    m_opacity = 0.0;
    update();
    m_animationGroup->start();
}

void ChapterTransition::setOpacity(qreal opacity)
{
    if (m_opacity != opacity) {
        m_opacity = opacity;
        update(); // 触发重绘
    }
}

void ChapterTransition::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setOpacity(m_opacity);

    // 绘制背景
    if (!m_background.isNull()) {
        painter.drawPixmap(rect(), m_background, m_background.rect());
    } else {
        // 如果没有背景图片，使用渐变色
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0, QColor(60, 30, 15, 255 * m_opacity));
        gradient.setColorAt(1, QColor(30, 10, 5, 255 * m_opacity));
        painter.fillRect(rect(), gradient);
    }

    // 父类绘制 (会绘制子控件，包括标题标签)
    QWidget::paintEvent(event);
}
