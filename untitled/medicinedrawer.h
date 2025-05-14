// medicinedrawer.h
#ifndef MEDICINEDRAWER_H
#define MEDICINEDRAWER_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

class MedicineDrawer : public QWidget
{
    Q_OBJECT

public:
    explicit MedicineDrawer(const QString &leftMedicine, const QString &rightMedicine,
                            QWidget *parent = nullptr);
    ~MedicineDrawer();

    // 获取左侧药材名称
    QString leftMedicine() const;

    // 获取右侧药材名称
    QString rightMedicine() const;

    // 设置抽屉状态（打开或关闭）
    void setOpen(bool isOpen);

    // 获取抽屉状态
    bool isOpen() const;

    // 翻转抽屉状态
    void toggle();

    // 检查抽屉是否包含指定药材
    bool containsMedicine(const QString &medicineName) const;

signals:
    // 点击药材时发出信号
    void medicineClicked(const QString &medicineName);

protected:
    // 重写绘制事件
    void paintEvent(QPaintEvent *event) override;

private:
    QPushButton *m_leftButton;    // 左侧药材按钮
    QPushButton *m_rightButton;   // 右侧药材按钮
    QLabel *m_stateLabel;         // 抽屉状态标签

    QString m_leftMedicine;       // 左侧药材名称
    QString m_rightMedicine;      // 右侧药材名称
    bool m_isOpen;                // 抽屉是否打开

    QPixmap m_drawerPixmap;       // 抽屉图片

    // 更新抽屉视觉效果
    void updateAppearance();

};

#endif // MEDICINEDRAWER_H
