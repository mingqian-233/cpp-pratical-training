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
    // 仅用于调试 - 添加到public部分
    QJsonObject getSaveData() const { return m_saveData; }

    static SaveManager* instance();

    // 删除拷贝构造函数和赋值操作符
    SaveManager(const SaveManager&) = delete;
    SaveManager& operator=(const SaveManager&) = delete;


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
    int getchallengeUnlockedLevel() const;
    void setchallengeUnlockedLevel(int level);

    // 获取特定关卡的最高分
    int getchallengeHighScore(int level) const;
    // 设置特定关卡的最高分
    void setchallengeHighScore(int level, int score);

    // 判断是否存在存档文件
    bool saveFileExists(const QString &filename = "save.json") const;

    // 获取当前存档文件路径
    QString getCurrentSaveFile() const { return m_currentSaveFile; }

private:
    QJsonObject m_saveData;
    QString m_currentSaveFile;

    // 初始化存档结构
    void initSaveStructure();
private:
    explicit SaveManager(QObject *parent = nullptr);
    static SaveManager* m_instance;
    // 在SaveManager类中添加
public:
    // 静态方法：仅读取存档信息，不影响当前实例
    static QString readSaveFileInfo(const QString &filePath);

};

#endif // SAVEMANAGER_H
/*
常见使用场景

1. 创建新游戏
// 创建新存档，设置玩家名称为"PlayerOne"
saveManager->createNewSave("PlayerOne");
saveManager->saveSaveFile();

2. 保存游戏进度
// 更新故事模式进度
saveManager->setStoryProgress(5);
// 更新自由模式高分
saveManager->setchallengeHighScore(2000);
// 保存到文件
saveManager->saveSaveFile();

3. 加载游戏
if (saveManager->loadSaveFile()) {
// 读取存档信息
QString playerName = saveManager->getPlayerName();
int storyProgress = saveManager->getStoryProgress();
int challengeProgress = saveManager->getchallengeProgress();
int highScore = saveManager->getchallengeHighScore();

// 根据读取的信息设置游戏状态
// ...
}

4. 使用不同的存档文件
// 加载指定名称的存档
saveManager->loadSaveFile("player2_save.json");

// 保存到指定名称的存档
saveManager->saveSaveFile("backup_save.json");

 */
