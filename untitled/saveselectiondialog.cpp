// saveselectdialog.cpp
#include "saveselectiondialog.h"
#include <QInputDialog>
#include <QStandardPaths>

SaveSelectDialog::SaveSelectDialog(QWidget *parent)
    : QDialog(parent), m_saveManager(new SaveManager(this))
{
    setWindowTitle(tr("选择存档"));
    setMinimumSize(400, 300);

    // 设置存档目录
    m_saveDirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/saves";
    QDir saveDir(m_saveDirPath);
    if (!saveDir.exists()) {
        saveDir.mkpath(".");
    }

    setupUI();
    connectSignals();
    scanSaveFiles();
}

SaveSelectDialog::~SaveSelectDialog()
{
}

QString SaveSelectDialog::getSelectedSaveFile() const
{
    return m_selectedSaveFile;
}

SaveManager* SaveSelectDialog::getSaveManager()
{
    return m_saveManager;
}

void SaveSelectDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel(tr("游戏存档"), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    m_saveListWidget = new QListWidget(this);
    m_saveListWidget->setAlternatingRowColors(true);

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    m_newSaveButton = new QPushButton(tr("新建存档"), this);
    m_loadSaveButton = new QPushButton(tr("加载存档"), this);
    m_deleteSaveButton = new QPushButton(tr("删除存档"), this);
    m_cancelButton = new QPushButton(tr("取消"), this);

    buttonLayout->addWidget(m_newSaveButton);
    buttonLayout->addWidget(m_loadSaveButton);
    buttonLayout->addWidget(m_deleteSaveButton);
    buttonLayout->addWidget(m_cancelButton);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(m_saveListWidget);
    mainLayout->addLayout(buttonLayout);

    // 初始状态下，如果没有存档，禁用加载和删除按钮
    m_loadSaveButton->setEnabled(false);
    m_deleteSaveButton->setEnabled(false);
}

void SaveSelectDialog::connectSignals()
{
    connect(m_newSaveButton, &QPushButton::clicked, this, &SaveSelectDialog::onNewSaveClicked);
    connect(m_loadSaveButton, &QPushButton::clicked, this, &SaveSelectDialog::onLoadSaveClicked);
    connect(m_deleteSaveButton, &QPushButton::clicked, this, &SaveSelectDialog::onDeleteSaveClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    connect(m_saveListWidget, &QListWidget::itemSelectionChanged, [this]() {
        bool hasSelection = !m_saveListWidget->selectedItems().isEmpty();
        m_loadSaveButton->setEnabled(hasSelection);
        m_deleteSaveButton->setEnabled(hasSelection);
    });

    connect(m_saveListWidget, &QListWidget::itemDoubleClicked,
            this, &SaveSelectDialog::onSaveItemDoubleClicked);
}

void SaveSelectDialog::scanSaveFiles()
{
    m_saveListWidget->clear();

    QDir saveDir(m_saveDirPath);
    QStringList filters;
    filters << "*.json";
    saveDir.setNameFilters(filters);

    QFileInfoList saveFiles = saveDir.entryInfoList(QDir::Files, QDir::Time);

    for (const QFileInfo &fileInfo : saveFiles) {
        QString saveInfo = getSaveInfo(fileInfo.absoluteFilePath());
        QListWidgetItem *item = new QListWidgetItem(saveInfo);
        item->setData(Qt::UserRole, fileInfo.absoluteFilePath());
        m_saveListWidget->addItem(item);
    }

    // 如果没有存档，禁用加载和删除按钮
    bool hasSaves = (m_saveListWidget->count() > 0);
    m_loadSaveButton->setEnabled(hasSaves);
    m_deleteSaveButton->setEnabled(hasSaves);
}

QString SaveSelectDialog::getSaveInfo(const QString &saveFilePath)
{
    SaveManager tempManager;
    if (tempManager.loadSaveFile(saveFilePath)) {
        QFileInfo fileInfo(saveFilePath);
        QString lastModified = fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss");

        // 获取已解锁的最高关卡
        int unlockedLevel = tempManager.getFreeUnlockedLevel();

        // 获取各关卡的最高分
        QString highScores;
        for (int i = 1; i <= unlockedLevel; i++) {
            if (i > 1) highScores += " | ";
            highScores += QString("关卡%1: %2分").arg(i).arg(tempManager.getFreeHighScore(i));
        }

        return QString("%1 - %2\n故事进度: %3 | 已解锁关卡: %4\n最高分: %5\n最后修改: %6")
            .arg(fileInfo.baseName())
            .arg(tempManager.getPlayerName())
            .arg(tempManager.getStoryProgress())
            .arg(unlockedLevel)
            .arg(highScores)
            .arg(lastModified);
    }

    return QFileInfo(saveFilePath).baseName() + tr(" (无法读取)");
}


void SaveSelectDialog::onNewSaveClicked()
{
    bool ok;
    QString playerName = QInputDialog::getText(this, tr("新建存档"),
                                               tr("请输入玩家名称:"), QLineEdit::Normal,
                                               tr("Player1"), &ok);
    if (ok && !playerName.isEmpty()) {
        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
        QString saveFileName = QString("%1/save_%2.json").arg(m_saveDirPath).arg(timestamp);

        m_saveManager->createNewSave(playerName);
        if (m_saveManager->saveSaveFile(saveFileName)) {
            m_selectedSaveFile = saveFileName;
            accept();
        } else {
            QMessageBox::critical(this, tr("错误"), tr("无法创建新存档!"));
        }
    }
    qDebug() << "存档目录路径:" << m_saveDirPath;

}

void SaveSelectDialog::onLoadSaveClicked()
{
    QList<QListWidgetItem*> selectedItems = m_saveListWidget->selectedItems();
    if (!selectedItems.isEmpty()) {
        QString saveFilePath = selectedItems.first()->data(Qt::UserRole).toString();
        if (m_saveManager->loadSaveFile(saveFilePath)) {
            m_selectedSaveFile = saveFilePath;
            accept();
        } else {
            QMessageBox::critical(this, tr("错误"), tr("无法加载所选存档!"));
        }
    }
}

void SaveSelectDialog::onDeleteSaveClicked()
{
    QList<QListWidgetItem*> selectedItems = m_saveListWidget->selectedItems();
    if (!selectedItems.isEmpty()) {
        QString saveFilePath = selectedItems.first()->data(Qt::UserRole).toString();

        QMessageBox::StandardButton reply = QMessageBox::question(this, tr("确认删除"),
                                                                  tr("确定要删除此存档吗？此操作不可恢复。"),
                                                                  QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            QFile file(saveFilePath);
            if (file.remove()) {
                refreshSaveList();
            } else {
                QMessageBox::critical(this, tr("错误"), tr("无法删除所选存档!"));
            }
        }
    }
}

void SaveSelectDialog::onSaveItemDoubleClicked(QListWidgetItem *item)
{
    QString saveFilePath = item->data(Qt::UserRole).toString();
    if (m_saveManager->loadSaveFile(saveFilePath)) {
        m_selectedSaveFile = saveFilePath;
        accept();
    } else {
        QMessageBox::critical(this, tr("错误"), tr("无法加载所选存档!"));
    }
}

void SaveSelectDialog::refreshSaveList()
{
    scanSaveFiles();
}
