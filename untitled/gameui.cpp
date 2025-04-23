#include "GameUI.h"
#include <QDebug>
#include <QFont>
#include <QPalette>

GameUI::GameUI(QWidget *parent)
    : QWidget(parent), m_cabinet(nullptr), m_medicineList(nullptr)
{
    setupUI();
}

GameUI::~GameUI()
{
   // 不需要删除m_cabinet和m_medicineList，因为它们是外部管理的
}

void GameUI::setupUI()
{
    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);

    // 创建标题
    QLabel* titleLabel = new QLabel("中药匹配游戏", this);
    QFont titleFont("微软雅黑", 16, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(titleLabel);

    // 创建水平布局，左侧为药材清单，右侧为药柜
    QHBoxLayout* contentLayout = new QHBoxLayout();

    // 创建药材清单区域
    m_medicineListArea = createMedicineListArea();
    contentLayout->addWidget(m_medicineListArea, 1);

    // 创建药柜区域
    QWidget* cabinetArea = createCabinetArea();
    contentLayout->addWidget(cabinetArea, 3);

    m_mainLayout->addLayout(contentLayout);

    // 设置窗口基本属性
    setMinimumSize(800, 600);
    setWindowTitle("中药匹配游戏");
}

QWidget* GameUI::createMedicineListArea()
{
    // 创建药材清单区域容器
    QWidget* listAreaWidget = new QWidget(this);
    QVBoxLayout* listLayout = new QVBoxLayout(listAreaWidget);

    // 创建标题
    QLabel* listTitle = new QLabel("药材清单", listAreaWidget);
    QFont titleFont("微软雅黑", 14, QFont::Bold);
    listTitle->setFont(titleFont);
    listTitle->setAlignment(Qt::AlignCenter);
    listLayout->addWidget(listTitle);

    // 创建药材清单列表
    m_medicineListWidget = new QListWidget(listAreaWidget);
    m_medicineListWidget->setStyleSheet("QListWidget { background-color: #f0f0e0; border: 1px solid #d0d0c0; }");
    listLayout->addWidget(m_medicineListWidget);

    return listAreaWidget;
}

QWidget* GameUI::createCabinetArea()
{
    // 创建药柜区域容器
    QWidget* cabinetAreaWidget = new QWidget(this);
    QVBoxLayout* cabinetAreaLayout = new QVBoxLayout(cabinetAreaWidget);

    // 创建标题
    QLabel* cabinetTitle = new QLabel("药柜", cabinetAreaWidget);
    QFont titleFont("微软雅黑", 14, QFont::Bold);
    cabinetTitle->setFont(titleFont);
    cabinetTitle->setAlignment(Qt::AlignCenter);
    cabinetAreaLayout->addWidget(cabinetTitle);

    // 创建药柜布局容器
    QWidget* cabinetWidget = new QWidget(cabinetAreaWidget);
    m_cabinetLayout = new QGridLayout(cabinetWidget);
    m_cabinetLayout->setSpacing(5);

    // 包装在滚动区域中以支持更大的药柜
    QScrollArea* scrollArea = new QScrollArea(cabinetAreaWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(cabinetWidget);

    cabinetAreaLayout->addWidget(scrollArea);

    return cabinetAreaWidget;
}

void GameUI::setGameData(Cabinet* cabinet, MedicineList* medicineList)
{
    m_cabinet = cabinet;
    m_medicineList = medicineList;

    // 连接药柜状态变化的信号
    if (m_cabinet) {
        connect(m_cabinet, &Cabinet::drawerStateChanged,
                this, &GameUI::onDrawerStateChanged);
    }

    // 初始化UI
    updateUI();
}

void GameUI::updateUI()
{
    if (!m_cabinet || !m_medicineList) {
        qDebug() << "Cabinet or MedicineList not set, cannot update UI";
        return;
    }

    updateMedicineListDisplay();
    updateCabinetDisplay();
}

void GameUI::updateMedicineListDisplay()
{
    if (!m_medicineList || !m_cabinet) {
        return;
    }

    // 清空当前列表
    m_medicineListWidget->clear();

    // 获取药材差异信息
    auto medicines = m_medicineList->getMedicines(*m_cabinet);

    // 添加药材到列表
    for (auto it = medicines.begin(); it != medicines.end(); ++it) {
        // 正确访问std::map迭代器中的键和值
        const QString& medicineName = it->first;  // 使用->first而不是.key()
        int currentCount = it->second.first;      // 使用->second而不是.value()
        int targetCount = it->second.second;

        // 创建药材项
        QString displayText = QString("%1 (%2/%3)").arg(medicineName).arg(currentCount).arg(targetCount);
        QListWidgetItem* item = new QListWidgetItem(displayText);

        // 根据是否满足要求设置颜色
        if (currentCount == targetCount) {
            item->setForeground(Qt::darkGreen);
        } else if (currentCount > targetCount) {
            item->setForeground(Qt::red);
        } else {
            item->setForeground(Qt::blue);
        }

        m_medicineListWidget->addItem(item);
    }
}

void GameUI::updateCabinetDisplay()
{
    if (!m_cabinet) {
        return;
    }

    // 清除现有布局中的所有项
    QLayoutItem *child;
    while ((child = m_cabinetLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    // 清空按钮数组
    m_drawerButtons.clear();

    // 初始化抽屉按钮数组
    int rows = m_cabinet->getRows();
    int cols = m_cabinet->getColumns();
    m_drawerButtons.resize(rows);

    // 创建抽屉网格
    for (int row = 0; row < rows; ++row) {
        m_drawerButtons[row].resize(cols);

        for (int col = 0; col < cols; ++col) {
            Drawer* drawer = m_cabinet->getDrawer(row, col);
            if (!drawer) continue;

            // 创建抽屉按钮
            QPushButton* drawerButton = new QPushButton();
            drawerButton->setFixedSize(100, 50);
            drawerButton->setStyleSheet("QPushButton { background-image: url(:/images/drawer.png); background-repeat: no-repeat; background-position: center; background-color: transparent; border: none; }");

            // 设置按钮文本显示药材名称
            const Medicine& med1 = drawer->getFirstMedicine();
            const Medicine& med2 = drawer->getSecondMedicine();
            QString buttonText = QString::fromStdString(med1.getName()) + "\n" +
                                 QString::fromStdString(med2.getName());

            drawerButton->setText(buttonText);

            // 根据抽屉状态设置文本颜色
            if (drawer->isOpen()) {
                drawerButton->setStyleSheet("QPushButton { background-image: url(:/images/drawer.png); background-repeat: no-repeat; background-position: center; background-color: red; color: white; border: none; }");
            } else {
                drawerButton->setStyleSheet("QPushButton { background-image: url(:/images/drawer.png); background-repeat: no-repeat; background-position: center; background-color: transparent; color: black; border: none; }");
            }

            // 存储药材名称作为用户数据，用于点击处理
            drawerButton->setProperty("medicine1", QString::fromStdString(med1.getName()));
            drawerButton->setProperty("medicine2", QString::fromStdString(med2.getName()));
            drawerButton->setProperty("row", row);
            drawerButton->setProperty("col", col);

            // 连接点击信号
            connect(drawerButton, &QPushButton::clicked, this, &GameUI::onMedicineButtonClicked);

            // 添加到布局
            m_cabinetLayout->addWidget(drawerButton, row, col);
            m_drawerButtons[row][col] = drawerButton;
        }
    }
}

void GameUI::onDrawerStateChanged(int row, int col, bool isOpen)
{
    // 确保坐标有效
    if (row < 0 || row >= m_drawerButtons.size() ||
        col < 0 || col >= m_drawerButtons[row].size()) {
        return;
    }

    // 更新对应的抽屉按钮样式
    QPushButton* button = m_drawerButtons[row][col];
    if (button) {
        if (isOpen) {
            button->setStyleSheet("QPushButton { background-image: url(:/images/drawer.png); background-repeat: no-repeat; background-position: center; background-color: red; color: white; border: none; }");
        } else {
            button->setStyleSheet("QPushButton { background-image: url(:/images/drawer.png); background-repeat: no-repeat; background-position: center; background-color: transparent; color: black; border: none; }");
        }
    }

    // 更新药材清单显示
    updateMedicineListDisplay();
}

void GameUI::onMedicineButtonClicked()
{
    // 获取发送者按钮
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    // 确定是点击的第一个还是第二个药材（简单实现为总是点击第一个）
    QString medicineName = button->property("medicine1").toString();

    // 发送药材点击信号
    emit medicineClicked(medicineName.toStdString());
}

