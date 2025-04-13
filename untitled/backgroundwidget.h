// backgroundwidget.h
#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QResizeEvent>
#include <QPainter>

class BackgroundWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BackgroundWidget(QWidget *parent = nullptr);
    ~BackgroundWidget();

            // 设置背景图片，默认为bg_main
    void setBackground(const QString &imageName = "bg_main");

protected:
    // 重写绘制事件，用于绘制背景
    void paintEvent(QPaintEvent *event) override;

    // 重写调整大小事件，用于处理窗口大小变化
    void resizeEvent(QResizeEvent *event) override;

private:
    QPixmap m_originalPixmap;    // 原始图片
    QPixmap m_scaledPixmap;      // 缩放后的图片
    QString m_currentImage;      // 当前图片名称

    // 更新缩放后的图片
    void updateScaledPixmap();
};

#endif // BACKGROUNDWIDGET_H

