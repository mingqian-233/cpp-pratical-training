#include <QApplication>
#include "mainwindow.h"
#include"musicmanager.h"
#include "setting.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置应用程序名称和组织名称
    a.setApplicationName("HerbalOdyssey");
    a.setOrganizationName("mingqian233");


    // 获取主窗口实例并显示
    MainWindow* mainWindow = MainWindow::instance();
    mainWindow->show();

    // 初始化设置单例，使用主窗口作为父窗口
    Setting::initialize(mainWindow);

    // 初始化全局音乐
    MusicManager::instance()->setMusic("taqing.wav");
    MusicManager::instance()->playMusic();

    return a.exec();  // 启动Qt事件循环
}
