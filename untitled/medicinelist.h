// medicinelist.h
#ifndef MEDICINELIST_H
#define MEDICINELIST_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QMap>
#include <QStringList>
#include <QObject>
class MedicineList : public QWidget
{
    Q_OBJECT

public:
    explicit MedicineList(QWidget *parent = nullptr);
    ~MedicineList();

    // 设置目标药材清单
    void setTargetList(const QMap<QString, int> &targetList);

    // 设置当前药材清单
    void setCurrentList(const QMap<QString, int> &currentList);

    // 获取目标药材清单
    QMap<QString, int> targetList() const;

    // 获取当前药材清单
    QMap<QString, int> currentList() const;

    // 检查当前清单是否符合目标清单
    bool isCompleted() const;

    void addExcluded(QString medicine);

    void updateDisplay();
private:
    QVBoxLayout *m_layout;                // 主布局
    QLabel *m_titleLabel;                 // 标题标签
    QMap<QString, int> m_targetList;      // 目标药材清单
    QMap<QString, int> m_currentList;     // 当前药材清单
    QList<QLabel*> m_medicineLabels;      // 药材标签列表
    QSet<QString> m_excludedMedicines;

    // 更新药材清单显示
};

#endif // MEDICINELIST_H
