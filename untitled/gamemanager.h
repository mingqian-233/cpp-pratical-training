#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include "Cabinet.h"
#include "MedicineList.h"
#include "Medicine.h"
#include "GameUI.h"

class GameManager : public QObject
{
    Q_OBJECT

public:
    // 单例模式
    static GameManager* instance();
    ~GameManager();

            // 游戏初始化方法
    void initializeGame(int rows, int columns, int medicineTypes, int initialOpenDrawers, int randomOperations);

    // 设置游戏界面
    void setGameUI(GameUI* gameUI);

    // 开始新游戏
    void startNewGame(int difficulty = 1);

    // 重置当前游戏
    void resetGame();

    // 检查游戏是否完成
    bool checkGameComplete();

    // 获取当前分数
    int getCurrentScore() const;

    // 获取当前难度级别
    int getCurrentDifficulty() const;

    // 获取已用步数
    int getStepCount() const;

public slots:
    // 处理药材点击事件
    void onMedicineClicked(const std::string& medicineName);

    // 处理抽屉状态变化
    void onDrawerStateChanged(int row, int col, bool isOpen);

    // 处理所有状态变化完成的事件
    void onAllStateChangesCompleted();

signals:
    // 游戏完成信号
    void gameCompleted(int score, int steps);

    // 游戏状态更新信号
    void gameStateUpdated(int currentScore, int steps);

private:
    // 私有构造函数 (单例模式)
    explicit GameManager(QObject* parent = nullptr);
    static GameManager* m_instance;

            // 游戏组件
    Cabinet* m_cabinet;
    MedicineList* m_medicineList;
    GameUI* m_gameUI;

    // 游戏状态
    int m_currentDifficulty;
    int m_stepCount;
    bool m_isGameActive;

    // 随机打乱药柜状态
    void randomizeDrawerOperations(int operations);

    // 计算分数
    int calculateScore() const;
};

#endif // GAMEMANAGER_H

