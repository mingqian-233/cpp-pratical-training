// savemanager.cpp
#include "savemanager.h"
#include <QDebug>

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

QString SaveManager::getPlayerName() const
{
    return m_saveData["user"].toObject()["name"].toString();
}

void SaveManager::setPlayerName(const QString &name)
{
    QJsonObject userObj = m_saveData["user"].toObject();
    userObj["name"] = name;
    m_saveData["user"] = userObj;
}

int SaveManager::getStoryProgress() const
{
    return m_saveData["story_mode"].toObject()["progress"].toInt();
}

void SaveManager::setStoryProgress(int progress)
{
    QJsonObject storyObj = m_saveData["story_mode"].toObject();
    storyObj["progress"] = progress;
    m_saveData["story_mode"] = storyObj;
}

int SaveManager::getFreeUnlockedLevel() const
{
    return m_saveData["free_mode"].toObject()["unlocked_level"].toInt();
}

void SaveManager::setFreeUnlockedLevel(int level)
{
    QJsonObject freeObj = m_saveData["free_mode"].toObject();
    freeObj["unlocked_level"] = level;
    m_saveData["free_mode"] = freeObj;
}

int SaveManager::getFreeHighScore(int level) const
{
    if (level < 1 || level > 8) {
        qDebug() << "无效的关卡编号:" << level;
        return 0;
    }

    QString levelKey = QString("level_%1_score").arg(level);
    return m_saveData["free_mode"].toObject()[levelKey].toInt();
}

void SaveManager::setFreeHighScore(int level, int score)
{
    if (level < 1 || level > 8) {
        qDebug() << "无效的关卡编号:" << level;
        return;
    }

    QJsonObject freeObj = m_saveData["free_mode"].toObject();
    QString levelKey = QString("level_%1_score").arg(level);
    freeObj[levelKey] = score;
    m_saveData["free_mode"] = freeObj;

    // 如果通过了当前关卡并且是最后一个解锁的关卡，解锁下一关
    if (level == getFreeUnlockedLevel() && level < 8) {
        setFreeUnlockedLevel(level + 1);
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

    // 自由模式数据 - 修改为支持8个关卡
    QJsonObject freeObj;
    freeObj["unlocked_level"] = 1;  // 初始只解锁第一关

    // 初始化8个关卡的最高分
    for (int i = 1; i <= 8; i++) {
        QString levelKey = QString("level_%1_score").arg(i);
        freeObj[levelKey] = 0;
    }

    m_saveData["free_mode"] = freeObj;
}
