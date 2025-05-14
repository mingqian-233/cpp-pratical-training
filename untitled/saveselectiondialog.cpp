// saveselectdialog.cpp
#include "saveselectiondialog.h"
#include <QInputDialog>
#include <QStandardPaths>
#include "qcoreapplication.h"

SaveSelectDialog::SaveSelectDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("选择存档"));
    setMinimumSize(400, 300);

    m_saveManager = SaveManager::instance();
    // 设置存档目录
    m_saveDirPath = QCoreApplication::applicationDirPath() + "/saves";

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
    QString info = SaveManager::readSaveFileInfo(saveFilePath);
    if (info.isEmpty()) {
        return QFileInfo(saveFilePath).baseName() + tr(" (无法读取)");
    }
    return info;
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

        // 添加调试信息
        qDebug() << "尝试加载存档文件:" << saveFilePath;
        qDebug() << "m_saveManager指针:" << m_saveManager;

        // 在加载之前检查m_saveData
        qDebug() << "加载前的m_saveManager数据状态:";
        if (m_saveManager) {
            qDebug() << "m_saveData是否为空:" << (m_saveManager->getSaveData().isEmpty() ? "是" : "否");
        } else {
            qDebug() << "m_saveManager为空!";
        }

        bool loadResult = false;
        try {
            loadResult = m_saveManager->loadSaveFile(saveFilePath);
            qDebug() << "加载结果:" << (loadResult ? "成功" : "失败");
        } catch (const std::exception& e) {
            qDebug() << "加载存档时捕获到标准异常:" << e.what();
            loadResult = false;
        } catch (...) {
            qDebug() << "加载存档时捕获到未知异常!";
            loadResult = false;
        }

        if (loadResult) {
            m_selectedSaveFile = saveFilePath;

            // 在成功加载后检查数据
            qDebug() << "加载后的存档数据:";
            qDebug() << "玩家名称:" << m_saveManager->getPlayerName();

            try {
                qDebug() << "故事进度:" << m_saveManager->getStoryProgress();
            } catch (...) {
                qDebug() << "获取故事进度时出错";
            }

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

    // 添加调试信息
    qDebug() << "双击 - 尝试加载存档:" << saveFilePath;

    try {
        if (m_saveManager->loadSaveFile(saveFilePath)) {
            m_selectedSaveFile = saveFilePath;

            // 调试 - 输出加载的数据
            qDebug() << "双击 - 加载成功，玩家名称:" << m_saveManager->getPlayerName();
            qDebug() << "双击 - 尝试获取故事进度...";

            try {
                int progress = m_saveManager->getStoryProgress();
                qDebug() << "双击 - 故事进度:" << progress;
            }
            catch (...) {
                qDebug() << "双击 - 获取故事进度时发生异常!";
            }

            accept();
        } else {
            QMessageBox::critical(this, tr("错误"), tr("无法加载所选存档!"));
        }
    }
    catch (const std::exception& e) {
        qDebug() << "双击 - 加载存档时异常:" << e.what();
        QMessageBox::critical(this, tr("错误"), tr("加载存档时出现异常!"));
    }
    catch (...) {
        qDebug() << "双击 - 加载存档时出现未知异常!";
        QMessageBox::critical(this, tr("错误"), tr("加载存档时出现未知异常!"));
    }
}


void SaveSelectDialog::refreshSaveList()
{
    scanSaveFiles();
}
