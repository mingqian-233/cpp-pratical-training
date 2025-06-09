#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QMessageBox>
#include <QDialog>
#include <QInputDialog>
#include <QDir>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QRegularExpression>
#include <QDateTime>
#include <QListWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QScreen>
#include <QGuiApplication>

// 动画相关头文件
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>
#include <QGraphicsDropShadowEffect>

#include "backgroundwidget.h"
#include "saveselectiondialog.h"
#include "savemanager.h"
#include "storymode.h"
#include "medicinegame.h"
#include "musicmanager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    void resizeEvent(QResizeEvent *event) override;

public:
    static MainWindow* instance();
    ~MainWindow();

    void switchToMainPage();
    void showBlackScreen();
private:
    void cleanupGamePages();  // 新增方法

private slots:
    void showSaveSelectDialog();
    void onStoryModeClicked();
    void onChallengeModeClicked();
    void onCustomModeClicked();
    void onGameRulesClicked();
    void onSettingsClicked();
    void onExitClicked();

private:
    explicit MainWindow(QWidget *parent = nullptr);
    static MainWindow* m_instance;

            // UI组件
    QStackedWidget* m_stackedWidget;
    QWidget* m_centralWidget;
    BackgroundWidget* m_backgroundWidget;
    QVBoxLayout* m_mainLayout;

            // 按钮
    QPushButton* m_storyModeBtn;
    QPushButton* m_challengeModeBtn;
    QPushButton* m_customModeBtn;
    QPushButton* m_gameRulesBtn;
    QPushButton* m_settingsBtn;
    QPushButton* m_exitBtn;

            // 动画相关组件
    QWidget* m_blackScreenWidget;       // 黑屏widget
    QWidget* m_logoWidget;              // logo显示widget
    QLabel* m_titleWidget;              // 游戏标题widget
    QGraphicsOpacityEffect* m_titleOpacity; // 标题透明度效果
    QList<QGraphicsOpacityEffect*> m_buttonOpacities; // 按钮透明度效果列表
    bool m_animationCompleted;          // 动画完成标志

            // 存档管理
    SaveManager* m_saveManager;

            // 私有方法
    void setupUI();
    QPushButton* createMenuButton(const QString& text);
    void loadChapter(int chapterNumber);
    void startChallengeLevel(int level);
    void createCustomLevel();
    void setupCustomDrawers(int rows, int cols, const QStringList& medicines);
    void loadCustomLevel();
    void startCustomGame(const QString& filePath);
    void designCustomGame(const QString& filePath);

            // 动画相关方法
    void startIntroAnimation();
    void showLogoAnimation();
    void showMainWindowAnimation();
    void createTitleWidget();
    void setupInitialState();
    void startMainWindowAnimations();
};

#endif // MAINWINDOW_H
