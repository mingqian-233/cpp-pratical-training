
# 设置项目名称和版本
TARGET = project1
TEMPLATE = app
VERSION = 0.1

# 设置 Qt 使用的模块
QT += core gui widgets multimedia


# 源文件列表（所有 .cpp 文件）
SOURCES += main.cpp \
           backgroundwidget.cpp \
           mainwindow.cpp \
           musicmanager.cpp \
           savemanager.cpp \
           saveselectiondialog.cpp \
           setting.cpp

# 头文件列表（所有 .h 文件）
HEADERS += backgroundwidget.h \
           mainwindow.h \
           musicmanager.h \
           savemanager.h \
           saveselectiondialog.h \
           setting.h

# 资源文件列表（.qrc 文件）
RESOURCES += resources.qrc

# 添加自动生成 .ui 文件的支持
CONFIG += c++17
INCLUDEPATH += .
DEPENDPATH += .

# 启用调试模式
CONFIG += debug

# 自动处理 .ui 文件
AUTOMOC = true
QMAKEGENERATEBUSY = true

# 应用程序运行时的属性
QMAKE_TARGET_DESCRIPTION = Game Runner

# 以下是额外的构建选项和配置
unix {
    # Linux/macOS 特定配置（如果您需要）
    # 可以在这里添加命令行参数或者环境配置
}

win32 {
    # Windows 特定配置（如果您需要）
    # 可以在这里添加特定于 Windows 的构建选项
    QMAKE_LFLAGS += /SUBSYSTEM:WINDOWS
}

macx {
    # macOS 特定配置（如果您需要）
    # 可以在这里添加特定于 macOS 的构建选项
}

# 如果您需要使用额外的依赖库或其他模块，可以添加在这里
# INCLUDEPATH += /path/to/external/library
# LIBS += -L/path/to/lib -lnameOfLibrary

# 如果您需要实现一些特殊的 Qt 配置，可以添加以下内容
QT_CONFIG += accessibility
QMAKE_TARGET_BUNDLE_VERSION = ${VERSION}

# 设置资源文件的加载路径
QMAKE_RC_FILE = resources.qrc

# 设置项目输出目录
CONFIG(release, debug|release) {
    DESTDIR = release
} else {
    DESTDIR = debug
}

# 其他自定义配置
# 例如，如果需要设置图标或者其它资源

DISTFILES += \
    C:/Users/tomfa/Desktop/·踏情 (已合并).wav
