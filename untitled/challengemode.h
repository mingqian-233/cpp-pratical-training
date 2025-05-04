#ifndef CHALLENGEMODE_H
#define CHALLENGEMODE_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include "GameManager.h"
#include "GameUI.h"
#include "backgroundwidget.h"
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include "backgroundwidget.h"
#include "savemanager.h"
#include <QStackedWidget>

class ChallengeMode : public QWidget
{
    Q_OBJECT

public:
    explicit ChallengeMode(QWidget *parent = nullptr);
    ~ChallengeMode();

private slots:
    void onLevelSelected();
    void onGameCompleted(int score, int steps);

signals:
    void returnToMainMenuRequested();  // 添加返回主菜单的信号

private:
    // UI组件
    QWidget* m_levelSelectWidget;
    GameUI* m_gameUI;
    QVector<QPushButton*> m_levelButtons;

    BackgroundWidget* m_backgroundWidget;
    // 当前选择的关卡
    int m_currentLevel;

    void setupUI();
    void createLevelSelectUI();
    void startLevel(int level);
    void getLevelParameters(int level, int& rows, int& columns,
                            int& medicineTypes, int& initialOpenDrawers,
                            int& randomOperations);
    void showLevelSelect();
    void showGame();
};

#endif // CHALLENGEMODE_H
