#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include "backgroundwidget.h"
#include "savemanager.h"
#include <QStackedWidget>
#include "challengemode.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // 获取单例实例
    static MainWindow* instance();
    ~MainWindow();

public:
    void switchToMainPage();

    // 在private slots部分添加：
private slots:
    // ... 现有的slots ...
    void showSaveSelectDialog();

    // 在private部分添加：
private:
    // ... 现有的成员 ...
    SaveManager* m_saveManager;

private slots:
    // 各个模式的点击响应函数
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
    ChallengeMode* m_challengeMode;
    QWidget* m_mainPage;  // 用于存放原有的主界面内容

};

#endif // MAINWINDOW_H
