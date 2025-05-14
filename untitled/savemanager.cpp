// savemanager.cpp
#include "savemanager.h"
#include <QDebug>
#include <QApplication>

SaveManager* SaveManager::m_instance = nullptr;

SaveManager* SaveManager::instance()
{
    if (!m_instance) {
        m_instance = new SaveManager(qApp);  // 使用qApp作为父对象确保程序退出时删除
    }
    return m_instance;
}

SaveManager::SaveManager(QObject *parent) : QObject(parent)
{
    initSaveStructure();
}

bool SaveManager::loadSaveFile(const QString &filename)
{
    QFile loadFile(filename);
    if (!loadFile.exists()) {
        qDebug() << "存档文件不存在:" << filename;
        return false;
    }

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开存档文件:" << filename;
        return false;
    }

    QByteArray saveData = loadFile.readAll();
    loadFile.close();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    if (loadDoc.isNull() || !loadDoc.isObject()) {
        qDebug() << "存档文件格式无效";
        return false;
    }

    m_saveData = loadDoc.object();
    m_currentSaveFile = filename;  // 保存当前文件路径
    return true;
}

bool SaveManager::saveSaveFile(const QString &filename)
{
    QFile saveFile(filename);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qDebug() << "无法创建存档文件:" << filename;
        return false;
    }

    QJsonDocument saveDoc(m_saveData);
    saveFile.write(saveDoc.toJson());
    saveFile.close();

    m_currentSaveFile = filename;  // 保存当前文件路径
    return true;
}

void SaveManager::createNewSave(const QString &playerName)
{
    initSaveStructure();
    setPlayerName(playerName);
}

void SaveManager::setPlayerName(const QString &name)
{
    QJsonObject userObj = m_saveData["user"].toObject();
    userObj["name"] = name;
    m_saveData["user"] = userObj;
}

int SaveManager::getStoryProgress() const
{
    // 添加安全检查
    if (!m_saveData.contains("story_mode")) {
        qDebug()<<"存档文件缺失";
        return 0; // 默认从第一章开始
    }

    QJsonObject storyObj = m_saveData["story_mode"].toObject();
    if (!storyObj.contains("progress")) {
        return 0;
    }

    return storyObj["progress"].toInt();
}


void SaveManager::setStoryProgress(int progress)
{
    QJsonObject storyObj = m_saveData["story_mode"].toObject();
    storyObj["progress"] = progress;
    m_saveData["story_mode"] = storyObj;
}

void SaveManager::setchallengeUnlockedLevel(int level)
{
    QJsonObject challengeObj = m_saveData["challenge_mode"].toObject();
    challengeObj["unlocked_level"] = level;
    m_saveData["challenge_mode"] = challengeObj;
}

void SaveManager::setchallengeHighScore(int level, int score)
{
    if (level < 1 || level > 9) {
        qDebug() << "无效的关卡编号:" << level;
        return;
    }

    QJsonObject challengeObj = m_saveData["challenge_mode"].toObject();
    QString levelKey = QString("level_%1_score").arg(level);
    challengeObj[levelKey] = score;
    m_saveData["challenge_mode"] = challengeObj;

    // 如果通过了当前关卡并且是最后一个解锁的关卡，解锁下一关
    if (level == getchallengeUnlockedLevel() && level < 9) {
        setchallengeUnlockedLevel(level + 1);
    }
}

bool SaveManager::saveFileExists(const QString &filename) const
{
    return QFile::exists(filename);
}

void SaveManager::initSaveStructure()
{
    // 创建基本的JSON结构
    m_saveData = QJsonObject();

    // 用户数据
    QJsonObject userObj;
    userObj["name"] = "Player1";
    m_saveData["user"] = userObj;

    // 故事模式数据
    QJsonObject storyObj;
    storyObj["progress"] = 0;
    m_saveData["story_mode"] = storyObj;

    // 自由模式数据 - 修改为支持9个关卡
    QJsonObject challengeObj;
    challengeObj["unlocked_level"] = 1;  // 初始只解锁第一关

    // 初始化9个关卡的最高分
    for (int i = 1; i <= 9; i++) {
        QString levelKey = QString("level_%1_score").arg(i);
        challengeObj[levelKey] = 0;
    }

    m_saveData["challenge_mode"] = challengeObj;
}
int SaveManager::getchallengeUnlockedLevel() const
{
    if (!m_saveData.contains("challenge_mode")) {
        return 1; // 默认为第一关
    }
    QJsonObject challengeObj = m_saveData["challenge_mode"].toObject();
    if (!challengeObj.contains("unlocked_level")) {
        return 1;
    }
    return challengeObj["unlocked_level"].toInt(1);
}

int SaveManager::getchallengeHighScore(int level) const
{
    if (!m_saveData.contains("challenge_mode")) {
        return 0;
    }
    QJsonObject challengeObj = m_saveData["challenge_mode"].toObject();
    QString levelKey = QString("level_%1_score").arg(level);
    if (!challengeObj.contains(levelKey)) {
        return 0;
    }
    return challengeObj[levelKey].toInt(0);
}

QString SaveManager::getPlayerName() const
{
    if (!m_saveData.contains("user")) {
        return "Player1";
    }
    QJsonObject userObj = m_saveData["user"].toObject();
    if (!userObj.contains("name")) {
        return "Player1";
    }
    return userObj["name"].toString("Player1");
}
// 实现静态方法
QString SaveManager::readSaveFileInfo(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        return QString();
    }

    QJsonObject saveData = doc.object();
    QFileInfo fileInfo(filePath);

            // 提取需要的信息
    QString playerName = "未知";
    int storyProgress = 0;
    int unlockedLevel = 1;
    QString highScores = "无";

            // 安全地获取玩家名称
    if (saveData.contains("user") && saveData["user"].isObject()) {
        QJsonObject userObj = saveData["user"].toObject();
        if (userObj.contains("name")) {
            playerName = userObj["name"].toString();
        }
    }

            // 安全地获取故事进度
    if (saveData.contains("story_mode") && saveData["story_mode"].isObject()) {
        QJsonObject storyObj = saveData["story_mode"].toObject();
        if (storyObj.contains("progress")) {
            storyProgress = storyObj["progress"].toInt();
        }
    }

            // 安全地获取挑战模式信息
    QString modeKey = saveData.contains("challenge_mode") ? "challenge_mode" :
                          (saveData.contains("free_mode") ? "free_mode" : "");

    if (!modeKey.isEmpty() && saveData[modeKey].isObject()) {
        QJsonObject modeObj = saveData[modeKey].toObject();
        if (modeObj.contains("unlocked_level")) {
            unlockedLevel = modeObj["unlocked_level"].toInt();
        }

                // 获取各关卡的最高分
        highScores.clear();
        for (int i = 1; i <= unlockedLevel; i++) {
            QString levelKey = QString("level_%1_score").arg(i);
            if (i > 1) highScores += " | ";

            int score = 0;
            if (modeObj.contains(levelKey)) {
                score = modeObj[levelKey].toInt();
            }

            highScores += QString("关卡%1: %2分").arg(i).arg(score);
        }
    }

    QString lastModified = fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss");

    return QString("%1 - %2\n故事进度: %3 | 已解锁关卡: %4\n最高分: %5\n最后修改: %6")
        .arg(fileInfo.baseName())
        .arg(playerName)
        .arg(storyProgress)
        .arg(unlockedLevel)
        .arg(highScores)
        .arg(lastModified);
}
