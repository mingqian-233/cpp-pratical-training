// medicinelist.cpp
#include "medicinelist.h"

MedicineList::MedicineList(QWidget *parent)
    : QWidget(parent)
{
    // Create the main layout
    m_layout = new QVBoxLayout(this);

    // Create the title label
    m_titleLabel = new QLabel("药材清单", this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #805030;");

    // Add title to layout
    m_layout->addWidget(m_titleLabel);
    m_layout->addSpacing(10);

    setLayout(m_layout);
    setMinimumWidth(200);

    // Set style
    setStyleSheet("background-color: rgba(240, 230, 220, 180); border-radius: 10px;");
}

MedicineList::~MedicineList()
{
}

void MedicineList::setTargetList(const QMap<QString, int> &targetList)
{
    m_targetList = targetList;
    updateDisplay();
}

void MedicineList::setCurrentList(const QMap<QString, int> &currentList)
{
    m_currentList = currentList;
    updateDisplay();
}

QMap<QString, int> MedicineList::targetList() const
{
    return m_targetList;
}

QMap<QString, int> MedicineList::currentList() const
{
    return m_currentList;
}
void MedicineList::addExcluded(QString medicine){
    m_excludedMedicines.insert(medicine);
}
bool MedicineList::isCompleted() const
{
    // Check if current list matches target list
    if (m_currentList.size() != m_targetList.size()) {
        return false;
    }

    for (auto it = m_targetList.begin(); it != m_targetList.end(); ++it) {
        if (!m_currentList.contains(it.key()) || m_currentList[it.key()] != it.value()) {
            return false;
        }
    }

    return true;
}



void MedicineList::updateDisplay()
{
    // Clear old medicine labels
    for (QLabel *label : m_medicineLabels) {
        m_layout->removeWidget(label);
        delete label;
    }
    m_medicineLabels.clear();

            // Create new medicine labels
    QStringList medicineNames = m_targetList.keys();
    for (const QString &name : medicineNames) {
        QString text = QString("%1: %2/%3").arg(name)
        .arg(m_currentList.value(name, 0))
            .arg(m_targetList.value(name, 0));

        QLabel *label = new QLabel(text, this);

                // Set label style
        if (m_currentList.value(name, 0) == m_targetList.value(name, 0)) {
            label->setStyleSheet("color: #008000; font-weight: bold;"); // Green for match
        } else if (m_currentList.value(name, 0) > m_targetList.value(name, 0)) {
            label->setStyleSheet("color: #800000; font-weight: bold;"); // Red for excess
        } else {
            label->setStyleSheet("color: #000080; font-weight: bold;"); // Blue for insufficient
        }
        // 如果药材被排除，则为其添加划掉效果
        if (m_excludedMedicines.contains(name)) {
            label->setStyleSheet("color: #ff0000; text-decoration: line-through; font-weight: bold;");
        }
        m_layout->addWidget(label);
        m_medicineLabels.append(label);
    }

            // Add bottom stretch space
    m_layout->addStretch();
}
