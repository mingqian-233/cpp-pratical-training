#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include "backgroundwidget.h"
#include "savemanager.h"
#include <QStackedWidget>
#include "medicinegame.h"
#include "medicinedrawer.h"
#include <QListWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include "qtimer.h"
#include "setting.h"
#include "saveselectiondialog.h"
#include "storymode.h"
#include "savemanager.h"
#include <qspinbox.h>
#include <QScrollArea>
#include <QSpinBox>
#include <QComboBox>
#include <QListWidget>
#include <QMap>
#include <QVector>
#include <QGridLayout>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QtMath>
#include <QFileDialog>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // 获取单例实例
    static MainWindow* instance();
    ~MainWindow();

public:
    void switchToMainPage();

private slots:
    void showSaveSelectDialog();

private:
    SaveManager* m_saveManager;

private slots:
    void onStoryModeClicked();
    void onChallengeModeClicked();
    void onCustomModeClicked();
    void onSettingsClicked();
    void onExitClicked();

private:
    // 私有构造函数（单例模式）
    explicit MainWindow(QWidget *parent = nullptr);
    static MainWindow* m_instance;

            // UI组件
    QWidget* m_centralWidget;
    BackgroundWidget* m_backgroundWidget;
    QVBoxLayout* m_mainLayout;

    // 按钮
    QPushButton* m_storyModeBtn;
    QPushButton* m_challengeModeBtn;
    QPushButton* m_customModeBtn;
    QPushButton* m_settingsBtn;
    QPushButton* m_exitBtn;


            // UI初始化函数
    void setupUI();

    // 创建菜单按钮的辅助函数
    QPushButton* createMenuButton(const QString& text);

//挑战模式
private:
    // 添加QStackedWidget和ChallengeMode
    QStackedWidget* m_stackedWidget;
    QWidget* m_mainPage;  // 用于存放原有的主界面内容
signals:
    void gameCompleted();  // 游戏完成信号

private:
    void startChallengeLevel(int level);  // 启动挑战关卡

    void createCustomLevel();
    void setupCustomDrawers(int rows, int cols, const QStringList& medicines);
    void loadCustomLevel();
    void startCustomGame(const QString& filePath);
    void designCustomGame(const QString& filePath);
    void loadChapter(int chapterNumber);

};

#endif // MAINWINDOW_H
