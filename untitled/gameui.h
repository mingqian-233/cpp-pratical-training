#ifndef GAMEUI_H
#define GAMEUI_H

#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QScrollArea>
#include <QPushButton>
#include <QMap>
#include "Cabinet.h"
#include "MedicineList.h"

class GameUI : public QWidget
{
    Q_OBJECT

public:
    explicit GameUI(QWidget *parent = nullptr);
    ~GameUI();

            // 设置药柜和药材清单
    void setGameData(Cabinet* cabinet, MedicineList* medicineList);

    // 更新UI显示
    void updateUI();

signals:
    // 当药材被点击时发出信号
    void medicineClicked(const std::string& medicineName);

private slots:
    // 处理药柜状态变化
    void onDrawerStateChanged(int row, int col, bool isOpen);

    // 处理药材点击事件
    void onMedicineButtonClicked();

private:
    // 初始化UI组件
    void setupUI();

    // 创建药材清单区域
    QWidget* createMedicineListArea();

    // 创建药柜区域
    QWidget* createCabinetArea();

    // 更新药材清单显示
    void updateMedicineListDisplay();

    // 更新药柜显示
    void updateCabinetDisplay();

            // 成员变量
    Cabinet* m_cabinet;                  // 药柜引用
    MedicineList* m_medicineList;        // 药材清单引用

    QVBoxLayout* m_mainLayout;           // 主布局
    QWidget* m_medicineListArea;         // 药材清单区域
    QListWidget* m_medicineListWidget;   // 药材清单组件
    QGridLayout* m_cabinetLayout;        // 药柜布局
    QMap<QString, QLabel*> m_medicineLabels; // 药材标签映射

    // 药柜按钮二维数组，用于存储界面上的抽屉按钮
    QVector<QVector<QPushButton*>> m_drawerButtons;
};

#endif // GAMEUI_H

