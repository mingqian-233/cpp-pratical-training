// ChapterTransition.h
#ifndef CHAPTERTRANSITION_H
#define CHAPTERTRANSITION_H

#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include "qevent.h"

class ChapterTransition : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit ChapterTransition(QWidget *parent = nullptr);

    void showChapter(const QString &chapterTitle);
    qreal opacity() const { return m_opacity; }
    void setOpacity(qreal opacity);

signals:
    void transitionFinished();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QLabel *m_titleLabel;
    QPixmap m_background;
    qreal m_opacity;
    QSequentialAnimationGroup *m_animationGroup;
    QPropertyAnimation *m_fadeInAnimation;
    QPropertyAnimation *m_holdAnimation;
    QPropertyAnimation *m_fadeOutAnimation;

protected:
    void mousePressEvent(QMouseEvent *event) override { event->accept(); }
    void mouseReleaseEvent(QMouseEvent *event) override { event->accept(); }

};

#endif // CHAPTERTRANSITION_H
