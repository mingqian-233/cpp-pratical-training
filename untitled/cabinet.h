#ifndef CABINET_H
#define CABINET_H

#include "Drawer.h"
#include <vector>
#include <map>
#include <QObject>
#include <random>
// 使用QObject作为基类s，以便支持信号和槽机制
class Cabinet : public QObject {
    Q_OBJECT
private:
    std::mt19937 rng; // 随机数生成器

public:
    // 构造函数
    explicit Cabinet(QObject* parent = nullptr);

    // 初始化药柜，创建指定数量的抽屉
    void initialize(int rows, int columns,int medicineTypeCount);

    // 获取指定位置的抽屉
    Drawer* getDrawer(int row, int col);

    // 获取所有抽屉
    const std::vector<Drawer>& getAllDrawers() const;

    // 处理药材被点击的事件
    void onMedicineClicked(const QString& medicineName);


    // 获取当前开启的抽屉中的药材及数量
    std::map<QString, int > getCurrentMedicines() const;
    // 获取药材清单
    std::map<QString, std::pair<int,int> > getMedicineList() const;
    // 获取药柜的行数和列数
    int getRows() const;
    int getColumns() const;

    // 随机设置一些抽屉的初始状态为打开
    void randomizeInitialState(int openDrawersCount);

    // 检查指定位置的抽屉是否打开
    bool isDrawerOpen(int row, int col) const;

signals:
    // 当抽屉状态改变时发出信号
    void drawerStateChanged(int row, int col, bool isOpen);

    // 当所有状态变化完成时发出信号
    void allStateChangesCompleted();

private:
    std::vector<Drawer> drawers;  // 所有抽屉
    int rows;                     // 药柜行数
    int columns;                  // 药柜列数

    // 药材名称到包含该药材的抽屉索引的映射
    std::map<QString, std::vector<int>> medicineToDrawers;

    // 初始化药材到抽屉的映射
    void initializeMedicineToDrawersMap();
};

#endif // CABINET_H
