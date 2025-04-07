#include <QApplication>
#include "mainmenu.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainMenu w;  // 创建主菜单界面
    w.show();  // 显示界面

    return a.exec();  // 启动Qt事件循环
}
