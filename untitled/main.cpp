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

            // 获取主窗口实例
    MainWindow* mainWindow = MainWindow::instance();

    // 先显示黑屏
    mainWindow->showBlackScreen();

    // 初始化设置单例
    Setting::initialize(mainWindow);

    // 应用设置中的全屏状态
    Setting* settings = Setting::instance();
    if (settings->getIsFullScreen()) {
        mainWindow->showFullScreen();
    } else {
        mainWindow->show();
        // 居中显示
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect screenGeometry = screen->availableGeometry();
            int x = (screenGeometry.width() - mainWindow->width()) / 2;
            int y = (screenGeometry.height() - mainWindow->height()) / 2;
            mainWindow->move(x, y);
        }
    }

            // 初始化全局音乐
    MusicManager::instance()->setMusic("taqing.mp3");
    MusicManager::instance()->playMusic();

    return a.exec();
}
