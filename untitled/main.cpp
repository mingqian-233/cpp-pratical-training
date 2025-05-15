#include <QApplication>
#include "mainwindow.h"
#include "musicmanager.h"
#include "setting.h"
#include <QString>
#include <qmessagebox.h>
#include <QtCore/QDebug>
#include <QtCore/QString>
int main(int argc, char *argv[])
{


    QApplication a(argc, argv);

    // 设置应用程序名称和组织名称
    a.setApplicationName("HerbalOdyssey");
    a.setOrganizationName("mingqian233");

    // 获取主窗口实例并显示
    MainWindow* mainWindow = MainWindow::instance();
    mainWindow->setFixedSize(mainWindow->size());
    // 获取屏幕尺寸
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();  // 获取屏幕可用区域
        int x = (screenGeometry.width() - mainWindow->width()) / 2;
        int y = (screenGeometry.height() - mainWindow->height()) / 2;
        mainWindow->move(x, y);  // 将窗口移动到屏幕中央
    }

    mainWindow->show();
    // 初始化设置单例，使用主窗口作为父窗口
    Setting::initialize(mainWindow);

            // 初始化全局音乐
    MusicManager::instance()->setMusic("taqing.mp3");
    MusicManager::instance()->playMusic();

    return a.exec();  // 启动Qt事件循环
}
