#include "GameManager.h"

GameManager* GameManager::m_instance = nullptr;

GameManager* GameManager::instance()
{
    if (!m_instance) {
        m_instance = new GameManager();
    }
    return m_instance;
}
void GameManager::cleanupCurrentGame()
{
    // 清理游戏状态
    m_isGameActive = false;
    m_stepCount = 0;

    // 断开所有信号连接
    disconnect(m_cabinet.get(), nullptr, this, nullptr);

    // 清理资源
    m_cabinet.reset();
    m_medicineList.reset();

    // 清理 UI 引用（但不删除 UI 对象）
    if (m_gameUI) {
        disconnect(m_gameUI, nullptr, this, nullptr);
        m_gameUI = nullptr;
    }
}

GameManager::GameManager(QObject* parent)
    : QObject(parent)
      , m_currentDifficulty(1)
      , m_stepCount(0)
      , m_isGameActive(false)
      , m_randomGenerator(QRandomGenerator::global()->generate())
{
}

GameManager::~GameManager()
{
    cleanupCurrentGame();
}
void GameManager::initializeGame(int rows, int columns, int medicineTypes,
                                 int initialOpenDrawers, int randomOperations)
{
    m_rows = rows;
    m_columns = columns;
    m_medicineTypes = medicineTypes;
    m_initialOpenDrawers = initialOpenDrawers;
    m_randomOperations = randomOperations;

            // 创建新的游戏组件
    m_cabinet = std::make_unique<Cabinet>();
    m_medicineList = std::make_unique<MedicineList>();

            // 初始化药柜
    m_cabinet->initialize(rows, columns,medicineTypes);

    auto r=m_cabinet->getCurrentMedicines();

            // 连接信号和槽
    connect(m_cabinet.get(), &Cabinet::drawerStateChanged,
            this, &GameManager::onDrawerStateChanged);
    connect(m_cabinet.get(), &Cabinet::allStateChangesCompleted,
            this, &GameManager::onAllStateChangesCompleted);
}

void GameManager::setGameUI(GameUI* gameUI)
{
    m_gameUI = gameUI;
    if (m_gameUI) {
        m_gameUI->setGameData(m_cabinet.get(), m_medicineList.get());
        connect(m_gameUI, &GameUI::medicineSideClicked,
                this, &GameManager::onMedicineClicked);
    }
}

void GameManager::startNewGame(int difficulty)
{
    m_currentDifficulty = difficulty;
    m_stepCount = 0;
    m_isGameActive = true;

    // 根据难度调整参数
    int operations = m_randomOperations * (difficulty+3);

    // 随机设置初始打开的抽屉
    m_cabinet->randomizeInitialState(m_initialOpenDrawers);

    // 根据当前状态生成目标药材清单
    m_medicineList->generateFromCabinetState(*m_cabinet);

    // 执行随机操作
    randomizeDrawerOperations(operations);

    // 更新UI
    if (m_gameUI) {
        m_gameUI->updateUI();
    }

    emit gameStateUpdated(getCurrentScore(), m_stepCount);
}

void GameManager::resetGame()
{
    startNewGame(m_currentDifficulty);
}

void GameManager::randomizeDrawerOperations(int operations)
{
    auto& medicines = MedicineManager::getInstance().getMedicines();
    for (int i = 0; i < operations; ++i) {
        int randomIndex = m_randomGenerator.bounded(medicines.size());
        const Medicine& medicine = medicines[randomIndex];
        m_cabinet->onMedicineClicked(medicine.getName());
    }
}

bool GameManager::checkGameComplete()
{
    if (!m_isGameActive) return false;
    return m_medicineList->isSatisfied(*m_cabinet);
}

int GameManager::calculateScore() const
{
    // 基础分数计算：1000分为基础，每多一步扣除10分
    int baseScore = 1000;
    int stepPenalty = m_stepCount * 10;
    return std::max(0, baseScore - stepPenalty);
}

void GameManager::onMedicineClicked(const QString& medicineName)
{
    if (!m_isGameActive) return;

    m_stepCount++;
    m_cabinet->onMedicineClicked(medicineName);

    emit gameStateUpdated(getCurrentScore(), m_stepCount);

    if (checkGameComplete()) {
        m_isGameActive = false;
        emit gameCompleted(getCurrentScore(), m_stepCount);
    }
}

void GameManager::onDrawerStateChanged(int row, int col, bool isOpen)
{
    if (m_gameUI) {
        m_gameUI->updateUI();
    }
}

void GameManager::onAllStateChangesCompleted()
{
    if (m_gameUI) {
        m_gameUI->updateUI();
    }
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


