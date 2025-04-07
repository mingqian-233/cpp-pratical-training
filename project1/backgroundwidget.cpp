// backgroundwidget.cpp
#include "backgroundwidget.h"
#include <QDebug>
#include <QApplication>
#include <QWindow>

BackgroundWidget::BackgroundWidget(QWidget *parent)
    : QWidget(parent)
      , m_currentImage("bg_main")
{
    // 设置默认背景
    setBackground(m_currentImage);

    // 确保widget能够接收到窗口状态变化事件
    setAttribute(Qt::WA_StyledBackground, true);

    // 设置追踪鼠标移动
    setMouseTracking(true);
}

BackgroundWidget::~BackgroundWidget()
{
}
void BackgroundWidget::setBackground(const QString &imageName)
{
    // 从资源文件加载图片
    QString imagePath = QString(":/images/%1.png").arg(imageName);

    if (m_originalPixmap.load(imagePath)) {
        qDebug() << "Successfully loaded background:" << imagePath
                 << "Size:" << m_originalPixmap.size();
    } else {
        qDebug() << "Failed to load background image:" << imagePath;
        // 尝试不同的路径格式
        imagePath = QString(":images/%1.png").arg(imageName);
        if (m_originalPixmap.load(imagePath)) {
            qDebug() << "Successfully loaded using alternate path:" << imagePath;
        } else {
            qDebug() << "All loading attempts failed. Check your resource paths.";
            return;
        }
    }

    m_currentImage = imageName;
    updateScaledPixmap();
    update(); // 触发重绘
}

// 修改paintEvent方法确保背景图片填满整个控件
void BackgroundWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    if (!m_scaledPixmap.isNull()) {
        // 绘制背景图片以填满整个控件
        painter.drawPixmap(rect(), m_scaledPixmap);
    } else {
        // 如果图片未能加载，填充一个红色背景作为提示
        painter.fillRect(rect(), Qt::red);
        qDebug() << "Warning: Background image is null";
    }
}

// 确保图片总是填满整个控件
void BackgroundWidget::updateScaledPixmap()
{
    if (!m_originalPixmap.isNull() && width() > 0 && height() > 0) {
        m_scaledPixmap = m_originalPixmap.scaled(
            size(),
            Qt::IgnoreAspectRatio,  // 忽略宽高比以填满整个窗口
            Qt::SmoothTransformation
            );
    }
}

void BackgroundWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    // 当窗口大小变化时，更新缩放的图片
    updateScaledPixmap();

    // 调用父类的resizeEvent
    QWidget::resizeEvent(event);
}
