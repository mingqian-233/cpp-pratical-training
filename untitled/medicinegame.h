// medicinegame.h
#ifndef MEDICINEGAME_H
#define MEDICINEGAME_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QSet>
#include <QStringList>
#include "backgroundwidget.h"
#include "medicinedrawer.h"
#include "medicinelist.h"
#include "musicmanager.h"
#include <QInputDialog>
#include <QTimer>
#include <QDateTime>
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QSet>
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>

#include "medicinedrawer.h"
#include "medicinelist.h"

struct MedicineDrawerState {
    QString leftMedicine;
    QString rightMedicine;
    bool isOpen;
};

struct MedicineDrawerConfig {
    int rows;
    int cols;
    QVector<MedicineDrawerState> initialStates;
    QMap<QString, int> targetList;
};

class MedicineGame : public QWidget
{
    Q_OBJECT
private:
    QRandomGenerator m_random;  // Our own random generator instance


public:
    explicit MedicineGame(QWidget *parent = nullptr,bool design=false);
    ~MedicineGame();

            // 初始化游戏，传入药材种类数、药柜行列数和随机操作次数
    void initGame(int medicineTypes, int rows, int cols, int operationCount,QString bg_pic="bg_challenge");

    // 切换背景图片
    void setBackground(const QString &imageName);


private slots:
    // 处理药材点击
    void onMedicineClicked(const QString &medicineName);

    // 显示答案
    void showAnswer();

protected:
    void resizeEvent(QResizeEvent *event) override;



private:
    BackgroundWidget *m_background;    // 背景
    MedicineList *m_medicineList;      // 药材清单
    QGridLayout *m_drawerLayout;       // 药柜布局
    QPushButton *m_answerButton;       // 答案按钮

    QVector<MedicineDrawer*> m_drawers;      // 所有抽屉
    QStringList m_medicineNames;             // 所有药材名称
    QVector<QString> m_operations;           // 随机操作序列
    QVector<QString> m_playerOperations;     // 玩家操作序列

    int m_rows;           // 药柜行数
    int m_cols;           // 药柜列数
    bool m_gameOver;      // 游戏是否结束

    // 从文件加载药材名称
    void loadMedicineNames();

    // 初始化药柜
    void initDrawers(const QStringList &selectedMedicines);

    // 生成药材清单
    void generateMedicineList();

    // 执行随机操作
    void performRandomOperations(int count);

    // 翻转所有包含指定药材的抽屉
    void toggleDrawers(const QString &medicineName);

    // 检查游戏是否结束
    bool checkGameOver();

    // 计算当前药材清单
    QMap<QString, int> calculateCurrentList();

// 添加万能药材功能

private:
    QPushButton *m_allPowerfulButton;    // 万能药材按钮
    QSet<QString> m_excludedMedicines;   // 被排除的药材集合

            // 显示药材选择对话框
    void showMedicineSelector();

signals:
    void gameCompleted();  // 游戏完成信号


private:
    // 添加以下成员变量
    bool m_designMode = false;
    QStringList m_designOperations;

public:
    //初始化自定义模式
    void initCustomGame(const QJsonObject& levelData);
    // 添加以下方法声明
    void designCustomGame(const QJsonObject& levelData);
    QJsonObject finishDesign(const QJsonObject& originalData);
    void onDesignMedicineClicked(const QString& medicineName);

signals:
    // 添加以下信号
    void designOperationAdded(const QString& operation);

};

#endif // MEDICINEGAME_H
