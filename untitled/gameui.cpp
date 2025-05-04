// GameUI.cpp
#include "GameUI.h"
#include <QDebug>
#include <QFont>
#include <QPalette>
#include <QTransform>
#include <QPainter>
#include <QImage>

GameUI::GameUI(QWidget *parent)
    : QWidget(parent), m_cabinet(nullptr), m_medicineList(nullptr)
{
    setupUI();
}

GameUI::~GameUI()
{
   // 不需要删除m_cabinet和m_medicineList，因为它们是外部管理的
}

void GameUI::reset()
{
    // 先将指针设为空，避免updateMedicineListDisplay访问无效指针
    m_cabinet = nullptr;
    m_medicineList = nullptr;

            // 清理所有动态创建的 UI 元素
    updateMedicineListDisplay();  // 清空药材列表显示
    updateCabinetDisplay();      // 清空药柜显示

            // 断开所有信号连接
    disconnect(this, nullptr, nullptr, nullptr);
}

void GameUI::setupUI()
{
    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

            // 创建标题
    QLabel* titleLabel = new QLabel("中药匹配游戏", this);
    QFont titleFont("微软雅黑", 16, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(titleLabel);

            // 创建水平布局，左侧为药材清单，右侧为药柜
    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(20);

            // 创建药材清单区域
    m_medicineListArea = createMedicineListArea();
    contentLayout->addWidget(m_medicineListArea, 1);

            // 创建药柜区域
    QWidget* cabinetArea = createCabinetArea();
    contentLayout->addWidget(cabinetArea, 3);

    m_mainLayout->addLayout(contentLayout, 1);

            // 设置窗口基本属性
    setMinimumSize(800, 600);
    setWindowTitle("中药匹配游戏");
}

QWidget* GameUI::createMedicineListArea()
{
    // 创建药材清单区域容器
    QWidget* listAreaWidget = new QWidget(this);
    QVBoxLayout* listLayout = new QVBoxLayout(listAreaWidget);
    listLayout->setContentsMargins(5, 5, 5, 5);
    listLayout->setSpacing(5);

            // 创建标题
    QLabel* listTitle = new QLabel("药材清单", listAreaWidget);
    QFont titleFont("微软雅黑", 14, QFont::Bold);
    listTitle->setFont(titleFont);
    listTitle->setAlignment(Qt::AlignCenter);
    listLayout->addWidget(listTitle);

            // 创建药材清单容器
    QWidget* medicineListWidget = new QWidget(listAreaWidget);
    medicineListWidget->setStyleSheet("background-color: #f0f0f0; border-radius: 5px;");
    m_medicineListLayout = new QVBoxLayout(medicineListWidget);
    m_medicineListLayout->setSpacing(5);
    m_medicineListLayout->setContentsMargins(5, 5, 5, 5);

            // 添加滚动区域
    QScrollArea* scrollArea = new QScrollArea(listAreaWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(medicineListWidget);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    listLayout->addWidget(scrollArea);

    return listAreaWidget;
}

QWidget* GameUI::createCabinetArea()
{
    // 创建药柜区域容器
    QWidget* cabinetAreaWidget = new QWidget(this);
    QVBoxLayout* cabinetAreaLayout = new QVBoxLayout(cabinetAreaWidget);
    cabinetAreaLayout->setContentsMargins(5, 5, 5, 5);
    cabinetAreaLayout->setSpacing(5);

            // 创建标题
    QLabel* cabinetTitle = new QLabel("药柜", cabinetAreaWidget);
    QFont titleFont("微软雅黑", 14, QFont::Bold);
    cabinetTitle->setFont(titleFont);
    cabinetTitle->setAlignment(Qt::AlignCenter);
    cabinetAreaLayout->addWidget(cabinetTitle);

            // 创建药柜容器 - 不使用滚动区域，直接使用固定大小的区域
    QWidget* cabinetWidget = new QWidget(cabinetAreaWidget);
    cabinetWidget->setStyleSheet("background-color: #f0f0f0; border-radius: 5px;");
    m_cabinetLayout = new QGridLayout(cabinetWidget);
    m_cabinetLayout->setSpacing(5);
    m_cabinetLayout->setContentsMargins(5, 5, 5, 5);

    cabinetAreaLayout->addWidget(cabinetWidget, 1);  // 设置为1的拉伸因子，使其填满可用空间

    return cabinetAreaWidget;
}

void GameUI::setGameData(Cabinet* cabinet, MedicineList* medicineList)
{
    // 断开旧连接
    if (m_cabinet) {
        disconnect(m_cabinet, nullptr, this, nullptr);
    }

    m_cabinet = cabinet;
    m_medicineList = medicineList;

            // 仅在cabinet有效时连接信号
    if (m_cabinet) {
        connect(m_cabinet, SIGNAL(drawerStateChanged(int,int,bool)),
                this, SLOT(onDrawerStateChanged(int,int,bool)),
                Qt::UniqueConnection);
    }

            // 仅在数据可用时初始化UI
    if (m_cabinet && m_medicineList) {
        updateUI();
    }
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
    if (!m_medicineList) {
        return;
    }

            // 清空现有布局中的所有项
    QLayoutItem *child;
    while ((child = m_medicineListLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

            // 获取所有药材
    auto allMedicines = m_medicineList->getMedicines();

            // 添加每种药材到列表中
    for (const auto& medicine : allMedicines) {
        // 创建药材容器
        QWidget* medicineItemWidget = new QWidget();
        QHBoxLayout* medicineLayout = new QHBoxLayout(medicineItemWidget);
        medicineLayout->setContentsMargins(5, 5, 5, 5);
        medicineLayout->setSpacing(10);

                // 药材名称标签
        QLabel* nameLabel = new QLabel(medicine.first);
        nameLabel->setFont(QFont("微软雅黑", 10));
        medicineLayout->addWidget(nameLabel);

                // 药材数量信息
        int currentCount = medicine.second.first;  // 当前数量
        int targetCount = medicine.second.second;  // 目标数量

        QString countText = QString("(%1/%2)").arg(currentCount).arg(targetCount);
        QLabel* countLabel = new QLabel(countText);
        countLabel->setFont(QFont("微软雅黑", 9));
        medicineLayout->addWidget(countLabel);

                // 创建按钮容器
        QWidget* buttonWidget = new QWidget();
        QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
        buttonLayout->setContentsMargins(0, 0, 0, 0);
        buttonLayout->setSpacing(5);

                // 创建左按钮
        QPushButton* leftButton = new QPushButton("左");
        leftButton->setFixedSize(30, 25);
        leftButton->setProperty("medicine", medicine.first);
        leftButton->setProperty("side", "left");
        connect(leftButton, &QPushButton::clicked, this, &GameUI::onMedicineSideButtonClicked);
        buttonLayout->addWidget(leftButton);

                // 创建右按钮
        QPushButton* rightButton = new QPushButton("右");
        rightButton->setFixedSize(30, 25);
        rightButton->setProperty("medicine", medicine.first);
        rightButton->setProperty("side", "right");
        connect(rightButton, &QPushButton::clicked, this, &GameUI::onMedicineSideButtonClicked);
        buttonLayout->addWidget(rightButton);

        medicineLayout->addWidget(buttonWidget);

                // 根据是否满足要求设置样式
        if (currentCount == targetCount) {
            medicineItemWidget->setStyleSheet("background-color: #e0f0e0; border-radius: 3px;");
        } else if (currentCount > targetCount) {
            medicineItemWidget->setStyleSheet("background-color: #ffe0e0; border-radius: 3px;");
        } else {
            medicineItemWidget->setStyleSheet("background-color: #e0e0ff; border-radius: 3px;");
        }

                // 将药材项添加到布局中
        m_medicineListLayout->addWidget(medicineItemWidget);
    }

            // 添加弹性空间
    m_medicineListLayout->addStretch();
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

            // 获取药柜布局的大小
    QSize layoutSize = m_cabinetLayout->parentWidget()->size();

    // 计算每个抽屉按钮的固定大小
    int buttonMaxWidth = (layoutSize.width() - 10) / cols;
    int buttonMaxHeight = (layoutSize.height() - 10) / rows;

    // 确保最小尺寸
    int buttonWidth = qMax(buttonMaxWidth, 60);
    int buttonHeight = qMax(buttonMaxHeight, 80);

    // 创建抽屉网格
    for (int row = 0; row < rows; ++row) {
        m_drawerButtons[row].resize(cols);

        for (int col = 0; col < cols; ++col) {
            Drawer* drawer = m_cabinet->getDrawer(row, col);
            if (!drawer) continue;

                    // 创建抽屉按钮
            QPushButton* drawerButton = new QPushButton();
            drawerButton->setFixedSize(buttonWidth, buttonHeight);

                    // 创建垂直布局，药材名称垂直显示
            QHBoxLayout* drawerLayout = new QHBoxLayout(drawerButton);
            drawerLayout->setContentsMargins(2, 2, 2, 2);
            drawerLayout->setSpacing(2);

                    // 获取药材
            const Medicine& med1 = drawer->getFirstMedicine();
            const Medicine& med2 = drawer->getSecondMedicine();

                    // 创建左侧药材标签（垂直文字）
            QLabel* med1Label = new QLabel();
            QString med1Text = med1.getName();
            QFont med1Font("微软雅黑", 9);

            // 创建右侧药材标签（垂直文字）
            QLabel* med2Label = new QLabel();
            QString med2Text = med2.getName();
            QFont med2Font("微软雅黑", 9);

            // 设置样式和文本内容
            if (drawer->isOpen()) {
                // 打开状态：红色背景，白色文字
                drawerButton->setStyleSheet("QPushButton { background-color: red; border: 1px solid black; }");
                med1Label->setStyleSheet("color: white;");
                med2Label->setStyleSheet("color: white;");
            } else {
                // 关闭状态：使用背景图片，黑色文字
                drawerButton->setStyleSheet("QPushButton { background-color: #c0a080; border: 1px solid black; }");
                med1Label->setStyleSheet("color: black;");
                med2Label->setStyleSheet("color: black;");
            }

            // 使用HTML格式实现垂直文本
            med1Label->setText("<div style='writing-mode: vertical-rl; text-orientation: mixed;'>" + med1Text + "</div>");
            med1Label->setAlignment(Qt::AlignCenter);
            med1Label->setFont(med1Font);

            med2Label->setText("<div style='writing-mode: vertical-rl; text-orientation: mixed;'>" + med2Text + "</div>");
            med2Label->setAlignment(Qt::AlignCenter);
            med2Label->setFont(med2Font);

            // 添加到抽屉布局
            drawerLayout->addWidget(med1Label, 1);
            drawerLayout->addWidget(med2Label, 1);

                    // 存储抽屉位置作为属性
            drawerButton->setProperty("row", row);
            drawerButton->setProperty("col", col);

                    // 连接点击信号
            connect(drawerButton, &QPushButton::clicked, this, &GameUI::onDrawerButtonClicked);

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
        // 找到按钮中的标签
        QList<QLabel*> labels = button->findChildren<QLabel*>();

        if (isOpen) {
            // 打开状态：红色背景，白色文字
            button->setStyleSheet("QPushButton { background-color: red; border: 1px solid black; }");
            for (QLabel* label : labels) {
                label->setStyleSheet("color: white;");
            }
        } else {
            // 关闭状态：使用背景图片，黑色文字
            button->setStyleSheet("QPushButton { background-color: #c0a080; border: 1px solid black; }");
            for (QLabel* label : labels) {
                label->setStyleSheet("color: black;");
            }
        }
    }

            // 更新药材清单显示
    if (m_medicineList) {
        m_medicineList->updateCurrentCount(*m_cabinet);
        updateMedicineListDisplay();
    }
}

void GameUI::onDrawerButtonClicked()
{
    // 获取发送者按钮
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

            // 获取抽屉位置
    int row = button->property("row").toInt();
    int col = button->property("col").toInt();

            // 通知游戏逻辑处理抽屉点击
    emit drawerClicked(row, col);
}

void GameUI::onMedicineSideButtonClicked()
{
    // 获取发送者按钮
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

            // 获取药材名称和侧面
    QString medicineName = button->property("medicine").toString();
    QString side = button->property("side").toString();

            // 发送药材点击信号，包含侧面信息
    emit medicineSideClicked(medicineName, side);
}
