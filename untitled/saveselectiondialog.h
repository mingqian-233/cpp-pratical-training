// saveselectdialog.h
#ifndef SAVESELECTDIALOG_H
#define SAVESELECTDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include "savemanager.h"

class SaveSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveSelectDialog(QWidget *parent = nullptr);
    ~SaveSelectDialog();

            // 获取选中的存档文件名
    QString getSelectedSaveFile() const;

    // 获取SaveManager实例
    SaveManager* getSaveManager();

private slots:
    void onNewSaveClicked();
    void onLoadSaveClicked();
    void onDeleteSaveClicked();
    void onSaveItemDoubleClicked(QListWidgetItem *item);
    void refreshSaveList();

private:
    QListWidget *m_saveListWidget;
    QPushButton *m_newSaveButton;
    QPushButton *m_loadSaveButton;
    QPushButton *m_deleteSaveButton;
    QPushButton *m_cancelButton;

    QString m_selectedSaveFile;
    QString m_saveDirPath;
    SaveManager *m_saveManager;

    void setupUI();
    void connectSignals();
    void scanSaveFiles();
    QString getSaveInfo(const QString &saveFilePath);
};

#endif // SAVESELECTDIALOG_H
