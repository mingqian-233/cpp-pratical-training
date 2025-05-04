#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QPointer>
#include <memory>
#include "Cabinet.h"
#include "MedicineList.h"
#include "GameUI.h"
#include <QRandomGenerator>

class GameManager : public QObject
{
    Q_OBJECT

public:
    // 获取单例实例
    static GameManager* instance();
    ~GameManager();

            // 初始化游戏基本参数
    void initializeGame(int rows, int columns, int medicineTypes,
                        int initialOpenDrawers, int randomOperations);

    // 设置游戏UI
    void setGameUI(GameUI* gameUI);

            // 开始新游戏，difficulty决定难度级别
    void startNewGame(int difficulty = 1);

            // 重置当前游戏到初始状态
    void resetGame();
            // 检查游戏是否完成
    bool checkGameComplete();

            // 获取游戏状态信息
    int getCurrentScore() const;
    int getCurrentDifficulty() const;
    int getStepCount() const;
    Cabinet* getCabinet() const { return m_cabinet.get(); }
    MedicineList* getMedicineList() const { return m_medicineList.get(); }

public slots:
    // 处理药材点击事件
    void onMedicineClicked(const QString& medicineName);

    // 处理抽屉状态变化事件
    void onDrawerStateChanged(int row, int col, bool isOpen);

    // 处理所有状态变化完成事件
    void onAllStateChangesCompleted();

            // 清理当前游戏资源
    void cleanupCurrentGame();
signals:
    // 游戏完成信号
    void gameCompleted(int score, int steps);

    // 游戏状态更新信号
    void gameStateUpdated(int currentScore, int steps);

private:
    explicit GameManager(QObject* parent = nullptr);
    static GameManager* m_instance;

    std::unique_ptr<Cabinet> m_cabinet;
    std::unique_ptr<MedicineList> m_medicineList;
    QPointer<GameUI> m_gameUI;

    int m_currentDifficulty;
    int m_stepCount;
    bool m_isGameActive;
    QRandomGenerator m_randomGenerator;

            // 游戏参数
    int m_rows;
    int m_columns;
    int m_medicineTypes;
    int m_initialOpenDrawers;
    int m_randomOperations;

            // 执行随机抽屉操作
    void randomizeDrawerOperations(int operations);

    // 计算当前得分
    int calculateScore() const;

};

#endif // GAMEMANAGER_H
