// savemanager.h
#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QString>
#include <QDir>

class SaveManager : public QObject
{
    Q_OBJECT
public:
    explicit SaveManager(QObject *parent = nullptr);

    // 加载存档
    bool loadSaveFile(const QString &filename = "save.json");

    // 保存存档
    bool saveSaveFile(const QString &filename = "save.json");

    // 创建新的存档
    void createNewSave(const QString &playerName = "Player1");

    // 获取玩家名称
    QString getPlayerName() const;

    // 设置玩家名称
    void setPlayerName(const QString &name);

    // 故事模式相关操作
    int getStoryProgress() const;
    void setStoryProgress(int progress);

    // 自由模式相关操作 - 获取已解锁的最高关卡
    int getFreeUnlockedLevel() const;
    void setFreeUnlockedLevel(int level);

    // 获取特定关卡的最高分
    int getFreeHighScore(int level) const;
    // 设置特定关卡的最高分
    void setFreeHighScore(int level, int score);

    // 判断是否存在存档文件
    bool saveFileExists(const QString &filename = "save.json") const;

    // 获取当前存档文件路径
    QString getCurrentSaveFile() const { return m_currentSaveFile; }

private:
    QJsonObject m_saveData;
    QString m_currentSaveFile;

    // 初始化存档结构
    void initSaveStructure();
};

#endif // SAVEMANAGER_H
