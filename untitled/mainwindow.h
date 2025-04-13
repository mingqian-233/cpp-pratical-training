#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include "backgroundwidget.h"

#include "savemanager.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    // 获取单例实例的静态方法
    static MainWindow* instance();

    // 禁止拷贝构造和赋值操作
    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onStoryModeClicked();
    void onChallengeModeClicked();
    void onCustomModeClicked();
    void onSettingsClicked();
    void onExitClicked();

private:
    // 私有构造函数，防止外部创建实例
    explicit MainWindow(QWidget *parent = nullptr);

    // 单例实例指针
    static MainWindow* m_instance;

    void setupUI();
    bool isFullScreen=0;

    BackgroundWidget* backgroundWidget;




private:
    SaveManager *m_saveManager;
    void showSaveSelectionDialog();
    void updateGameState();

};

#endif // MAINWINDOW_H
