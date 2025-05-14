// medicinedrawer.cpp
#include "medicinedrawer.h"

MedicineDrawer::MedicineDrawer(const QString &leftMedicine, const QString &rightMedicine,
                               QWidget *parent)
    : QWidget(parent)
      , m_leftMedicine(leftMedicine)
      , m_rightMedicine(rightMedicine)
      , m_isOpen(false)
{
    // 加载抽屉图片
    m_drawerPixmap.load(":/images/drawer.png");
    if (m_drawerPixmap.isNull()) {
        qDebug() << "Failed to load drawer image!";
    }

    // 创建主布局
    QHBoxLayout *layout = new QHBoxLayout(this);

    // 创建左侧药材按钮
    m_leftButton = new QPushButton(m_leftMedicine, this);
    m_leftButton->setFixedSize(60, 60);
    m_leftButton->setStyleSheet("background-color: transparent; border: none; color: white; font-weight: bold;");
    connect(m_leftButton, &QPushButton::clicked, this, [this]() {
        emit medicineClicked(m_leftMedicine);
    });

    // 创建状态标签
    m_stateLabel = new QLabel(this);
    m_stateLabel->setFixedSize(20, 60);
    m_stateLabel->setStyleSheet("background-color: transparent;");

    // 创建右侧药材按钮
    m_rightButton = new QPushButton(m_rightMedicine, this);
    m_rightButton->setFixedSize(60, 60);
    m_rightButton->setStyleSheet("background-color: transparent; border: none; color: white; font-weight: bold;");
    connect(m_rightButton, &QPushButton::clicked, this, [this]() {
        emit medicineClicked(m_rightMedicine);
    });

    // 添加组件到布局
    layout->addWidget(m_leftButton);
    layout->addWidget(m_stateLabel);
    layout->addWidget(m_rightButton);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(2);

    setLayout(layout);
    setFixedSize(150, 70);

    // 更新外观
    updateAppearance();
}

MedicineDrawer::~MedicineDrawer()
{
}

QString MedicineDrawer::leftMedicine() const
{
    return m_leftMedicine;
}

QString MedicineDrawer::rightMedicine() const
{
    return m_rightMedicine;
}

void MedicineDrawer::setOpen(bool isOpen)
{
    if (m_isOpen != isOpen) {
        m_isOpen = isOpen;
        updateAppearance();
        update(); // 触发重绘
    }
}

bool MedicineDrawer::isOpen() const
{
    return m_isOpen;
}

void MedicineDrawer::toggle()
{
    m_isOpen = !m_isOpen;
    updateAppearance();
    update(); // 触发重绘
}

bool MedicineDrawer::containsMedicine(const QString &medicineName) const
{
    return m_leftMedicine == medicineName || m_rightMedicine == medicineName;
}

void MedicineDrawer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // 绘制抽屉图片作为背景
    if (!m_drawerPixmap.isNull()) {
        painter.drawPixmap(rect(), m_drawerPixmap);
    }

    // 调用父类的绘制方法以显示子部件
    QWidget::paintEvent(event);
}

void MedicineDrawer::updateAppearance()
{
    // 设置状态标签
    if (m_isOpen) {
        m_stateLabel->setText("◄►"); // 打开状态
        m_stateLabel->setStyleSheet("color: #80ff80; font-weight: bold; background-color: transparent;"); // 绿色表示打开
    } else {
        m_stateLabel->setText("►◄"); // 关闭状态
        m_stateLabel->setStyleSheet("color: #ff8080; font-weight: bold; background-color: transparent;"); // 红色表示关闭
    }
}
