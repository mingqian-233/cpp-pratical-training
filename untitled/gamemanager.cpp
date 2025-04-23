#include "GameManager.h"
#include "medicine.h"
#include <QDebug>
#include <QRandomGenerator>
#include <QTime>
#include <algorithm>
#include <vector>

// 初始化静态单例指针
GameManager* GameManager::m_instance = nullptr;

GameManager* GameManager::instance()
{
    if (!m_instance) {
        m_instance = new GameManager();
    }
    return m_instance;
}

GameManager::GameManager(QObject* parent)
    : QObject(parent), m_cabinet(nullptr), m_medicineList(nullptr), m_gameUI(nullptr),
      m_currentDifficulty(1), m_stepCount(0), m_isGameActive(false)
{
    // 初始化随机数生成器种子
    QRandomGenerator::global()->seed(QTime::currentTime().msecsSinceStartOfDay());
}

GameManager::~GameManager()
{
    // 清理资源
    if (m_cabinet) {
        delete m_cabinet;
        m_cabinet = nullptr;
    }

    if (m_medicineList) {
        delete m_medicineList;
        m_medicineList = nullptr;
    }

    // GameUI不由GameManager拥有，不需要删除
}

void GameManager::initializeGame(int rows, int columns, int medicineTypes, int initialOpenDrawers, int randomOperations)
{
    // 清理旧资源
    if (m_cabinet) {
        delete m_cabinet;
    }

    if (m_medicineList) {
        delete m_medicineList;
    }

    // 创建新的药柜和药材清单
    m_cabinet = new Cabinet(this);
    m_medicineList = new MedicineList();

    // 初始化药柜大小
    m_cabinet->initialize(rows, columns);

    // 获取随机药材
    std::vector<Medicine> randomMedicines = MedicineManager::getInstance().getRandomMedicines(medicineTypes);

    // 确保有足够的药材填满所有抽屉
    int totalDrawers = rows * columns;
    std::vector<Medicine> allDrawerMedicines;

    // 每个抽屉需要两种药材，因此需要totalDrawers * 2的药材
    // 可能需要重复使用某些药材
    while (allDrawerMedicines.size() < totalDrawers * 2) {
        for (const auto& medicine : randomMedicines) {
            if (allDrawerMedicines.size() < totalDrawers * 2) {
                allDrawerMedicines.push_back(medicine);
            } else {
                break;
            }
        }
    }

    // 随机打乱药材顺序

    // 将药材分配到抽屉中
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < columns; ++col) {
            int index = row * columns + col;
            int medicineIndex1 = index * 2;
            int medicineIndex2 = index * 2 + 1;

            if (medicineIndex2 < allDrawerMedicines.size()) {
                Drawer* drawer = m_cabinet->getDrawer(row, col);
                if (drawer) {
                    drawer->setState(false); // 初始设置为关闭状态
                }
            }
        }
    }

    // 随机开启一些抽屉作为初始状态
    m_cabinet->randomizeInitialState(initialOpenDrawers);

    // 记录当前状态为目标状态
    m_medicineList->generateFromCabinetState(*m_cabinet);

    // 随机操作多次，打乱药柜状态
    randomizeDrawerOperations(randomOperations);

    // 重置步数
    m_stepCount = 0;

    // 设置游戏为活动状态
    m_isGameActive = true;

    // 更新UI显示
    if (m_gameUI) {
        m_gameUI->setGameData(m_cabinet, m_medicineList);
        m_gameUI->updateUI();
    }

    // 连接信号和槽
    connect(m_cabinet, &Cabinet::drawerStateChanged, this, &GameManager::onDrawerStateChanged);
    connect(m_cabinet, &Cabinet::allStateChangesCompleted, this, &GameManager::onAllStateChangesCompleted);

    qDebug() << "Game initialized with:"
             << "rows:" << rows
             << "columns:" << columns
             << "medicine types:" << medicineTypes
             << "initial open drawers:" << initialOpenDrawers
             << "random operations:" << randomOperations;
}

void GameManager::setGameUI(GameUI* gameUI)
{
    m_gameUI = gameUI;

    if (m_gameUI && m_cabinet && m_medicineList) {
        // 设置游戏数据
        m_gameUI->setGameData(m_cabinet, m_medicineList);

        // 连接药材点击信号
        connect(m_gameUI, &GameUI::medicineClicked, this, &GameManager::onMedicineClicked);
    }
}

void GameManager::startNewGame(int difficulty)
{
    m_currentDifficulty = difficulty;

    // 根据难度设置游戏参数
    int rows = 4 + difficulty;
    int columns = 4 + difficulty;
    int medicineTypes = 6 + difficulty * 2;
    int initialOpenDrawers = 3 + difficulty;
    int randomOperations = 5 + difficulty * 3;

    // 初始化游戏
    initializeGame(rows, columns, medicineTypes, initialOpenDrawers, randomOperations);
}

void GameManager::resetGame()
{
    // 重新初始化当前难度的游戏
    startNewGame(m_currentDifficulty);
}

bool GameManager::checkGameComplete()
{
    if (!m_cabinet || !m_medicineList) {
        return false;
    }

    // 检查当前药柜状态是否满足药材清单要求
    return m_medicineList->isSatisfied(*m_cabinet);
}

int GameManager::getCurrentScore() const
{
    return calculateScore();
}

int GameManager::getCurrentDifficulty() const
{
    return m_currentDifficulty;
}

int GameManager::getStepCount() const
{
    return m_stepCount;
}

void GameManager::onMedicineClicked(const std::string& medicineName)
{
    if (!m_isGameActive || !m_cabinet) {
        return;
    }

    // 处理药材点击事件
    m_cabinet->onMedicineClicked(medicineName);

    // 增加步数
    m_stepCount++;

    // 发送游戏状态更新信号
    emit gameStateUpdated(calculateScore(), m_stepCount);
}

void GameManager::onDrawerStateChanged(int row, int col, bool isOpen)
{
    qDebug() << "Drawer state changed at" << row << "," << col << "to" << (isOpen ? "open" : "closed");

    // 检查游戏是否完成
    if (checkGameComplete()) {
        m_isGameActive = false;
        emit gameCompleted(calculateScore(), m_stepCount);
    }
}

void GameManager::onAllStateChangesCompleted()
{
    // 所有抽屉状态变化完成后的处理
    qDebug() << "All drawer state changes completed";

    // 检查游戏是否完成
    if (checkGameComplete()) {
        m_isGameActive = false;
        emit gameCompleted(calculateScore(), m_stepCount);
    }
}

void GameManager::randomizeDrawerOperations(int operations)
{
    if (!m_cabinet) {
        return;
    }

    // 获取所有可用的药材名称
    std::vector<std::string> medicineNames;
    const auto& drawers = m_cabinet->getAllDrawers();

    for (const auto& drawer : drawers) {
        medicineNames.push_back(drawer.getFirstMedicine().getName());
        medicineNames.push_back(drawer.getSecondMedicine().getName());
    }

    // 去除重复的药材名称
    std::sort(medicineNames.begin(), medicineNames.end());
    medicineNames.erase(std::unique(medicineNames.begin(), medicineNames.end()), medicineNames.end());

    // 随机进行指定次数的操作
    for (int i = 0; i < operations; ++i) {
        // 随机选择一个药材
        int index = QRandomGenerator::global()->bounded(static_cast<int>(medicineNames.size()));
        std::string medicineName = medicineNames[index];

        // 点击该药材，改变包含它的所有抽屉状态
        m_cabinet->onMedicineClicked(medicineName);
    }
}

int GameManager::calculateScore() const
{
    if (!m_isGameActive || !m_cabinet || !m_medicineList) {
        return 0;
    }

    // 获取药材差异
    auto differences = m_medicineList->calculateDifference(*m_cabinet);

    // 计算差异总数
    int totalDifference = 0;
    for (const auto& diff : differences) {
        totalDifference += std::abs(diff.second.first - diff.second.second);
    }

    // 基础分数 - 根据难度和步数计算
    int baseScore = 1000 * m_currentDifficulty;

    // 步数惩罚
    int stepPenalty = m_stepCount * 10;

    // 差异惩罚
    int differencePenalty = totalDifference * 50;

    // 总分 = 基础分 - 步数惩罚 - 差异惩罚
    int score = baseScore - stepPenalty - differencePenalty;

    // 确保分数不为负
    return std::max(0, score);
}

