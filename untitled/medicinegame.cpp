// medicinegame.cpp
#include "medicinegame.h"

MedicineGame::MedicineGame(QWidget *parent,bool design)
    : QWidget(parent)

      , m_random(QRandomGenerator::securelySeeded())
      , m_background(nullptr)
      , m_medicineList(nullptr)
      , m_drawerLayout(nullptr)
      , m_answerButton(nullptr)
      , m_rows(0)
      , m_cols(0)
      , m_gameOver(false)
{
    loadMedicineNames();

    // Create main layout
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Create background widget and set it as the parent widget's background
    m_background = new BackgroundWidget(this);
    m_background->setGeometry(0, 0, width(), height());
    m_background->lower(); // Make sure it's behind all other widgets

    // Create medicine list
    m_medicineList = new MedicineList();
    m_medicineList->setFixedWidth(250);
    m_medicineList->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    // Create drawer container
    QWidget *drawerContainer = new QWidget();
    drawerContainer->setObjectName("drawerContainer");
    drawerContainer->setStyleSheet("#drawerContainer { background-color: rgba(255, 255, 255, 100); border-radius: 20px; }");
    drawerContainer->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
        );
    m_drawerLayout = new QGridLayout(drawerContainer);
    m_drawerLayout->setSpacing(5);
    // Add medicine list and drawer container to main layout
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(drawerContainer);

    mainLayout->addWidget(m_medicineList);
    mainLayout->addLayout(rightLayout);
    // 设置第 0 列（m_medicineList） stretch=0，第 1 列（rightLayout） stretch=1
    mainLayout->setStretch(0, 0);  // 左侧不拉伸
    mainLayout->setStretch(1, 1);  // 右侧占满剩余空间
    if(!design){

        // 在 MedicineGame 构造函数里，替换原有对 m_answerButton 和 m_allPowerfulButton 的添加方式：

        // 新建一个横向布局，用来并排放置两个按钮
        QHBoxLayout* toolButtonsLayout = new QHBoxLayout();

        // 窥天镜按钮
        m_answerButton = new QPushButton("窥天镜");
        m_answerButton->setIcon(QIcon(":/images/aethergazer.png"));
        m_answerButton->setFixedSize(300, 200);
        m_answerButton->setIconSize(QSize(150,150));
        m_answerButton->setStyleSheet(
            "QPushButton { background-color: #f0d0a0; "
            "border: 2px solid #805030; border-radius: 5px; "
            "color: #805030; font-weight: bold; font-size: 20px; }"
            );

        m_answerButton->setToolTip("可以窥探世界上一切事物的真理的神器");
        connect(m_answerButton, &QPushButton::clicked, this, &MedicineGame::showAnswer);
        toolButtonsLayout->addWidget(m_answerButton);

        // 万能药材按钮
        m_allPowerfulButton = new QPushButton("万能药材");
        m_allPowerfulButton->setIcon(QIcon(":/images/all-powerful.png"));
        m_allPowerfulButton->setFixedSize(300, 200);
        m_allPowerfulButton->setIconSize(QSize(150,150));
        m_allPowerfulButton->setStyleSheet(
            "QPushButton { background-color: #f0d0a0; "
            "border: 2px solid #805030; border-radius: 5px; "
            "color: #805030; font-weight: bold; font-size: 20px; }"
            );
        m_allPowerfulButton->setToolTip("神奇的药材，可以替代或者抵消任意数量的世界上任何一种药材");
        connect(m_allPowerfulButton, &QPushButton::clicked, this, &MedicineGame::showMedicineSelector);
        toolButtonsLayout->addWidget(m_allPowerfulButton);

        // 最后再把整个横向布局添加到 rightLayout
        rightLayout->addLayout(toolButtonsLayout, Qt::AlignCenter);

    }

}


void MedicineGame::showMedicineSelector()
{
    // 从当前的目标清单里拿出所有药材名

    QMap<QString,int> targets = m_medicineList->targetList();



    for (const QString &med : m_excludedMedicines) {
        targets.remove(med);
    }

    QStringList choices = targets.keys();

    if (choices.isEmpty()) {
        QMessageBox::information(this, "提示", "当前没有可排除的药材");
        return;
    }

    bool ok = false;
    QString selectedMedicine = QInputDialog::getItem(
        this,
        QStringLiteral("选择排除药材"),
        QStringLiteral("请选择要排除的药材："),
        choices,
        0,
        false,
        &ok
        );
    if (!ok || selectedMedicine.isEmpty())
        return;

            // 将用户选择的药材标记为已排除

    m_excludedMedicines.insert(selectedMedicine);
    m_medicineList->addExcluded(selectedMedicine);
    m_medicineList->updateDisplay();

    qDebug() << "万能药材已排除：" << selectedMedicine;
    if (checkGameOver()) {
        m_gameOver = true;
        // 播放胜利音效
        MusicManager::instance()->playEffect("win.wav");

                // 显示胜利消息
        QLabel *winLabel = new QLabel("恭喜你，成功完成任务！", this);
        winLabel->setStyleSheet("font-size: 24px; color: #008000; background-color: rgba(255, 255, 255, 180);");
        winLabel->setAlignment(Qt::AlignCenter);
        winLabel->setGeometry(width() / 4, height() / 2 - 50, width() / 2, 100);
        winLabel->show();
    }
}


// Add this method to MedicineGame class
void MedicineGame::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event); // Call the base class implementation

    // Update background size when the widget is resized
    if (m_background) {
        m_background->setGeometry(0, 0, width(), height());
    }
}

MedicineGame::~MedicineGame()
{
    // 清理资源
    for (MedicineDrawer *drawer : m_drawers) {
        delete drawer;
    }
    m_drawers.clear();
}


void MedicineGame::initGame(int medicineTypes, int rows, int cols, int operationCount,QString bg_pic)
{
    // Set parameters
    m_rows = rows;
    m_cols = cols;
    m_gameOver = false;

    // Clean up existing drawers
    for (MedicineDrawer *drawer : m_drawers) {
        delete drawer;
    }
    m_drawers.clear();
    m_operations.clear();
    m_playerOperations.clear();

    // Ensure we have exactly the specified number of medicine types
    QStringList selectedMedicines;
    if (m_medicineNames.size() < medicineTypes) {
        qDebug() << "Warning: Not enough medicine names loaded!";
        selectedMedicines = m_medicineNames;
    } else {
        // Shuffle the medicine names
        QStringList shuffledNames = m_medicineNames;
        for (int i = shuffledNames.size() - 1; i > 0; --i) {
            int j = m_random.bounded(i + 1);
            shuffledNames.swapItemsAt(i, j);
        }

        // Select exactly the specified number of medicine types
        for (int i = 0; i < medicineTypes && i < shuffledNames.size(); ++i) {
            selectedMedicines.append(shuffledNames[i]);
        }
    }

    qDebug() << "Selected medicines:" << selectedMedicines;

    // Initialize drawers with only the selected medicine types
    initDrawers(selectedMedicines);

    // Generate medicine list
    generateMedicineList();

    // Perform random operations
    performRandomOperations(operationCount);


            // Set background
    setBackground("bg_challenge");
}

// Updated initDrawers method to use only selected medicines
void MedicineGame::initDrawers(const QStringList &selectedMedicines)
{
    // Ensure medicine names are selected
    if (selectedMedicines.isEmpty()) {
        qDebug() << "No medicine names selected!";
        return;
    }

    // Create drawers
    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            // Select two different medicines from the selected list
            QString leftMedicine, rightMedicine;

            // Randomly select left medicine
            leftMedicine = selectedMedicines[m_random.bounded(selectedMedicines.size())];

            // Select right medicine (ensure it's different from left)
            do {
                rightMedicine = selectedMedicines[m_random.bounded(selectedMedicines.size())];
            } while (rightMedicine == leftMedicine);

            // Create drawer
            MedicineDrawer *drawer = new MedicineDrawer(leftMedicine, rightMedicine);
drawer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            // Randomly set drawer initial state
            drawer->setOpen(m_random.bounded(2) == 0);

            // Connect signal
            connect(drawer, &MedicineDrawer::medicineClicked, this, &MedicineGame::onMedicineClicked);

            // Add to layout
            m_drawerLayout->addWidget(drawer, row, col);

            // Add to drawer list
            m_drawers.append(drawer);
        }
    }
    for (int c = 0; c < m_cols; ++c) {
        m_drawerLayout->setColumnStretch(c, 1);
    }
    for (int r = 0; r < m_rows; ++r) {
        m_drawerLayout->setRowStretch(r, 1);
    }
}
void MedicineGame::setBackground(const QString &imageName)
{
    if (m_background) {
        m_background->setBackground(imageName);
    }
}



void MedicineGame::onMedicineClicked(const QString &medicineName)
{
    // 如果游戏已结束，则不处理点击
    if (m_gameOver) {
        return;
    }

    // 播放点击音效
    MusicManager::instance()->playEffect("click.mp3");

    // 翻转包含该药材的抽屉
    toggleDrawers(medicineName);

    // 记录玩家操作
    if(medicineName==m_operations.back())
        m_operations.erase(m_operations.end()-1);
    else{
        if(m_playerOperations.empty()||m_playerOperations.back()!=medicineName)
            m_playerOperations.append(medicineName);
        else m_playerOperations.erase(m_playerOperations.end()-1);
    }

    // 更新药材清单
    m_medicineList->setCurrentList(calculateCurrentList());

    // 检查游戏是否结束
    if (checkGameOver()) {
        m_gameOver = true;
        // 播放胜利音效
        MusicManager::instance()->playEffect("win.wav");

        // 显示胜利消息
        QLabel *winLabel = new QLabel("恭喜你，成功完成任务！", this);
        winLabel->setStyleSheet("font-size: 24px; color: #008000; background-color: rgba(255, 255, 255, 180);");
        winLabel->setAlignment(Qt::AlignCenter);
        winLabel->setGeometry(width() / 4, height() / 2 - 50, width() / 2, 100);
        winLabel->show();
    }
}

void MedicineGame::showAnswer()
{
    // 创建一个可移动的答案窗口
    QWidget *answerWidget = new QWidget(this);
    answerWidget->setWindowFlags(Qt::SubWindow);
    answerWidget->setAttribute(Qt::WA_DeleteOnClose);
    answerWidget->setStyleSheet("background-color: rgba(255, 255, 255, 220); border: 1px solid #808080; border-radius: 10px;");

    // 设置初始位置和大小
    answerWidget->setGeometry(width() / 6, height() / 2 - 100, width() * 2 / 3, 200);

    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(answerWidget);

    // 添加标题和关闭按钮
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("答案提示", answerWidget);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #404040;");

    QPushButton *closeButton = new QPushButton("×", answerWidget);
    closeButton->setFixedSize(24, 24);
    closeButton->setStyleSheet("QPushButton {background-color: #ff6060; color: white; border-radius: 12px; font-weight: bold;}"
        "QPushButton:hover {background-color: #ff8080;}");
    connect(closeButton, &QPushButton::clicked, answerWidget, &QWidget::close);

    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(closeButton);

    // 添加答案内容
    QString answer = "正确操作顺序：\n";
    for (int i = m_playerOperations.size() - 1; i >= 0; --i) {
        answer += m_playerOperations[i];
        answer += " → ";
    }

    for (int i = m_operations.size() - 1; i >= 0; --i) {
        answer += m_operations[i];
        if (i > 0) {
            answer += " → ";
        }
    }

    QLabel *answerLabel = new QLabel(answer, answerWidget);
    answerLabel->setStyleSheet("font-size: 16px; color: #000080;");
    answerLabel->setAlignment(Qt::AlignCenter);
    answerLabel->setWordWrap(true);

    // 将组件添加到布局
    layout->addLayout(titleLayout);
    layout->addWidget(answerLabel);

    // 添加自定义事件过滤器来实现拖拽功能
    class DragFilter : public QObject {
    public:
        DragFilter(QWidget *widget) : QObject(widget), m_widget(widget), m_dragging(false) {}

        bool eventFilter(QObject *obj, QEvent *event) override {
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::LeftButton) {
                    m_dragPosition = mouseEvent->globalPosition().toPoint() - m_widget->frameGeometry().topLeft();
                    m_dragging = true;
                    return true;
                }
            } else if (event->type() == QEvent::MouseMove) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if (m_dragging) {
                    m_widget->move(mouseEvent->globalPosition().toPoint() - m_dragPosition);
                    return true;
                }
            } else if (event->type() == QEvent::MouseButtonRelease) {
                if (m_dragging) {
                    m_dragging = false;
                    return true;
                }
            }
            return QObject::eventFilter(obj, event);
        }

    private:
        QWidget *m_widget;
        QPoint m_dragPosition;
        bool m_dragging;
    };

    // 安装事件过滤器
    DragFilter *filter = new DragFilter(answerWidget);
    answerWidget->installEventFilter(filter);

    // 显示答案窗口
    answerWidget->show();

    // 播放提示音效
    MusicManager::instance()->playEffect("tip.wav");
}


void MedicineGame::loadMedicineNames()
{
    // 从文件加载药材名称
    QFile file(":/data/medicine_name.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);


        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) {
                m_medicineNames.append(line);
            }
        }

        file.close();
    } else {
        qDebug() << "Failed to open medicine name file!";

        // 使用一些默认的药材名称
        m_medicineNames = QStringList() << "人参" << "当归" << "黄芪" << "甘草" << "茯苓"
                                        << "白术" << "川芎" << "熟地" << "干姜" << "陈皮"
                                        << "枸杞" << "菊花" << "红花" << "薄荷" << "桂枝"
                                        << "苍术" << "荆芥" << "防风" << "连翘" << "柴胡";
    }
}

void MedicineGame::generateMedicineList()
{
    // 计算当前药材清单
    QMap<QString, int> targetList = calculateCurrentList();

    // 设置目标药材清单
    m_medicineList->setTargetList(targetList);
    m_medicineList->setCurrentList(targetList);
}

void MedicineGame::performRandomOperations(int count)
{
    // Ensure drawers are initialized
    if (m_drawers.isEmpty()) {
        return;
    }

    // Save initial state
    QVector<bool> initialStates;
    for (MedicineDrawer *drawer : m_drawers) {
        initialStates.append(drawer->isOpen());
    }

    // Collect all medicines
    QSet<QString> allMedicines;
    for (MedicineDrawer *drawer : m_drawers) {
        allMedicines.insert(drawer->leftMedicine());
        allMedicines.insert(drawer->rightMedicine());
    }

    QStringList medicineList = allMedicines.values();

    bool isDifferent = false;
    while (!isDifferent) {
        // Clear operation record
        m_operations.clear();

        // Reset drawer states
        for (int i = 0; i < m_drawers.size(); ++i) {
            m_drawers[i]->setOpen(initialStates[i]);
        }

        // Perform random operations
        for (int i = 0; i < count; ++i) {
            // Randomly select a medicine

            QString medicine = medicineList[m_random.bounded(medicineList.size())];

            while(!m_operations.empty()&&medicine==m_operations.back())medicine = medicineList[m_random.bounded(medicineList.size())];

            // Toggle drawers containing this medicine
            toggleDrawers(medicine);

            // Record operation
            m_operations.append(medicine);
        }

        // Check if state changed
        isDifferent = false;
        for (int i = 0; i < m_drawers.size(); ++i) {
            if (m_drawers[i]->isOpen() != initialStates[i]) {
                isDifferent = true;
                break;
            }
        }
    }

    // Update current medicine list
    m_medicineList->setCurrentList(calculateCurrentList());
}
void MedicineGame::toggleDrawers(const QString &medicineName)
{
    // 翻转所有包含指定药材的抽屉
    for (MedicineDrawer *drawer : m_drawers) {
        if (drawer->containsMedicine(medicineName)) {
            drawer->toggle();
        }
    }
}
bool MedicineGame::checkGameOver()
{
    auto current = m_medicineList->currentList();
    auto target  = m_medicineList->targetList();

            // 跳过所有被排除的药材
    for (const QString &med : m_excludedMedicines) {
        current.remove(med);
        target.remove(med);
    }
    if (current == target) {
        emit gameCompleted();  // 发送游戏成功信号
        return true;// 只有当两张表一致时才算过关
    }
    else return false;
}


QMap<QString, int> MedicineGame::calculateCurrentList()
{
    QMap<QString, int> list;

    // 计算当前打开的抽屉中的药材
    for (MedicineDrawer *drawer : m_drawers) {
        if (drawer->isOpen()) {
            QString leftMedicine = drawer->leftMedicine();
            QString rightMedicine = drawer->rightMedicine();

            list[leftMedicine] = list.value(leftMedicine, 0) + 1;
            list[rightMedicine] = list.value(rightMedicine, 0) + 1;
        }
    }

    return list;
}


// 这个方法应该更简单，只关注游戏逻辑而不是UI
void MedicineGame::designCustomGame(const QJsonObject& levelData)
{
    // 清理现有状态
    for (MedicineDrawer *drawer : m_drawers) {
        delete drawer;
    }
    m_drawers.clear();
    m_operations.clear();
    m_playerOperations.clear();
    m_excludedMedicines.clear();
    m_gameOver = false;

    // 读取行列数
    m_rows = levelData["rows"].toInt();
    m_cols = levelData["cols"].toInt();

    // 处理抽屉数据
    QJsonArray drawersData = levelData["drawers"].toArray();

    // 创建抽屉
    for (const QJsonValue& drawerValue : drawersData) {
        QJsonObject drawer = drawerValue.toObject();
        int row = drawer["row"].toInt();
        int col = drawer["col"].toInt();
        QString leftMedicine = drawer["leftMedicine"].toString();
        QString rightMedicine = drawer["rightMedicine"].toString();
        bool isOpen = drawer["isOpen"].toBool();

        // 创建抽屉对象
        MedicineDrawer* drawerObj = new MedicineDrawer(leftMedicine, rightMedicine);
        drawerObj->setOpen(isOpen);

        // 连接信号
        connect(drawerObj, &MedicineDrawer::medicineClicked, this, &MedicineGame::onDesignMedicineClicked);

        // 添加到网格
        m_drawerLayout->addWidget(drawerObj, row, col);

        // 添加到抽屉列表
        m_drawers.append(drawerObj);
    }

    // 设置目标药材清单
    QMap<QString, int> targetList;
    QJsonArray medicineListData = levelData["medicineList"].toArray();
    for (const QJsonValue& medValue : medicineListData) {
        QJsonObject med = medValue.toObject();
        QString name = med["name"].toString();
        int count = med["count"].toInt();
        targetList[name] = count;
    }

    // 设置药材清单
    m_medicineList->setTargetList(targetList);
    m_medicineList->setCurrentList(calculateCurrentList());

    // 设置背景
    setBackground("bg_custom");


    // 将设计模式设置为true，表示现在是设计模式
    m_designMode = true;

    // 清空设计操作记录
    m_designOperations.clear();
}

// 添加设计模式下的点击处理
void MedicineGame::onDesignMedicineClicked(const QString& medicineName)
{
    if (!m_designMode) return;

    // 播放点击音效
    MusicManager::instance()->playEffect("click.mp3");

    // 翻转包含该药材的抽屉
    toggleDrawers(medicineName);

    // 记录操作
    m_designOperations.append(medicineName);

    // 发送设计操作信号，让外部UI可以更新
    emit designOperationAdded(medicineName);

    // 更新药材清单
    m_medicineList->setCurrentList(calculateCurrentList());
}

// 完成设计
QJsonObject MedicineGame::finishDesign(const QJsonObject& originalData)
{
    if (m_designOperations.isEmpty()) {
        return QJsonObject(); // 返回空对象表示没有操作
    }

    // 创建更新的关卡数据
    QJsonObject updatedLevelData = originalData;

    // 添加自定义操作记录
    QJsonArray operationsArray;
    for (const QString& operation : m_designOperations) {
        operationsArray.append(operation);
    }
    updatedLevelData["custom_operations"] = operationsArray;

    // 更新药柜状态
    QJsonArray updatedDrawers;
    for (int i = 0; i < m_drawers.size(); ++i) {
        MedicineDrawer* drawer = m_drawers[i];
        QJsonObject drawerObj;
        drawerObj["row"] = i / m_cols;
        drawerObj["col"] = i % m_cols;
        drawerObj["leftMedicine"] = drawer->leftMedicine();
        drawerObj["rightMedicine"] = drawer->rightMedicine();
        drawerObj["isOpen"] = drawer->isOpen();
        updatedDrawers.append(drawerObj);
    }
    updatedLevelData["drawers"] = updatedDrawers;


    // 关闭设计模式
    m_designMode = false;

    return updatedLevelData;
}
void MedicineGame::initCustomGame(const QJsonObject& levelData)
{
    // 清理现有状态
    for (MedicineDrawer *drawer : m_drawers) {
        delete drawer;
    }
    m_drawers.clear();
    m_operations.clear();
    m_playerOperations.clear();
    m_excludedMedicines.clear();
    m_gameOver = false;

            // 读取行列数
    m_rows = levelData["rows"].toInt();
    m_cols = levelData["cols"].toInt();

            // 处理抽屉数据
    QJsonArray drawersData = levelData["drawers"].toArray();

            // 创建抽屉
    for (const QJsonValue& drawerValue : drawersData) {
        QJsonObject drawer = drawerValue.toObject();
        int row = drawer["row"].toInt();
        int col = drawer["col"].toInt();
        QString leftMedicine = drawer["leftMedicine"].toString();
        QString rightMedicine = drawer["rightMedicine"].toString();
        bool isOpen = drawer["isOpen"].toBool();

                // 创建抽屉对象
        MedicineDrawer* drawerObj = new MedicineDrawer(leftMedicine, rightMedicine);
        drawerObj->setOpen(isOpen);

                // 连接信号
        connect(drawerObj, &MedicineDrawer::medicineClicked, this, &MedicineGame::onMedicineClicked);

                // 添加到网格
        m_drawerLayout->addWidget(drawerObj, row, col);

                // 添加到抽屉列表
        m_drawers.append(drawerObj);
    }

            // 设置目标药材清单
    QMap<QString, int> targetList;
    QJsonArray medicineListData = levelData["medicineList"].toArray();
    for (const QJsonValue& medValue : medicineListData) {
        QJsonObject med = medValue.toObject();
        QString name = med["name"].toString();
        int count = med["count"].toInt();
        targetList[name] = count;
    }

            // 设置药材清单
    m_medicineList->setTargetList(targetList);

    // 读入关卡文件中的custom_operation作为operation
    if (levelData.contains("custom_operations")) {
        QJsonArray operationsArray = levelData["custom_operations"].toArray();
        m_operations.clear();

        // 读取自定义操作
        for (const QJsonValue& opValue : operationsArray) {
            QString operation = opValue.toString();
            m_operations.append(operation);
        }

        qDebug() << "加载自定义操作：" << m_operations;
    } else {
        qWarning() << "没有找到自定义操作记录！";
    }

    // 更新当前药材清单
    m_medicineList->setCurrentList(calculateCurrentList());

    // 设置背景
    setBackground("bg_custom");

    // 确保不是设计模式
    m_designMode = false;
}
