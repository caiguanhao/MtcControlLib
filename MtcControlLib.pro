QT -= gui
QT += core serialport multimedia

TEMPLATE = lib
DEFINES += MTCCONTROLLIB_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Serial.cpp \
    camapi.cpp \
    camera.cpp \
    commands/data/mtc_core.cpp \
    commands/data/mtc_export.cpp \
    commands/data/mtc_interface.cpp \
    commands/data/mtc_package.cpp \
    consts.cpp \
    devapi.cpp \
    device.cpp \
    stdafx.cpp

HEADERS += \
    Serial.h \
    camapi.h \
    camera.h \
    commands/data/mtc_core.h \
    commands/data/mtc_export.h \
    commands/data/mtc_interface.h \
    commands/data/mtc_package.h \
    consts.h \
    devapi.h \
    device.h \
    mtclib.h \
    stdafx.h

linux {
    linux-g++ | linux-g++-64 | linux-g++-32 | linux-clang {
        message("Linux build")
        CONFIG  += LinuxBuild
        DEFINES += LinuxBuild
        linux-clang {
            message("Linux clang")
            QMAKE_CXXFLAGS += -Qunused-arguments -fcolor-diagnostics
        }
    } else {
        error("Unsuported Linux toolchain, only GCC 32- or 64-bit is supported")
    }
} else : win32 {

        message("Windows build")
        CONFIG += WindowsBuild
        DEFINES += WindowsBuild
} else {
    error("Unsupported build platform, only Linux, Windows are supported")
}

message(Qt version $$[QT_VERSION])

##Default rules for deployment.
#unix {
#    target.path = /usr/lib
#}
#!isEmpty(target.path): INSTALLS += target

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

WindowsBuild{
    message(+++++++++++++++++++++++++++++++++++++++++++++++)
    message(Windows libs files)
    message(+++++++++++++++++++++++++++++++++++++++++++++++)
#    LIBS += -L../lib/win64 -lavutil \
#    -lavcodec \
#    -lswscale \
#    -lavformat \
#    -lavdevice \
#    -lswresample\
#    -lavfilter
    LIBS += -L$$PWD/lib/win64/ -lavutil \
    -lavcodec \
    -lswscale \
    -lavformat \
    -lavdevice \
    -lswresample\
    -lavfilter
    RC_FILE = MtcControlLib.rc
#    LIBS += -L$$PWD/lib/win64/ -lavutil
#    LIBS += -L$$PWD/lib/win64/ -lavcodec
#    LIBS += -L$$PWD/lib/win64/ -lswscale
#    LIBS += -L$$PWD/lib/win64/ -lavformat
#    LIBS += -L$$PWD/lib/win64/ -lavdevice

#    LIBS += -L$$PWD/lib/win64/ -lswresample
#    LIBS += -L$$PWD/lib/win64/ -lavfilter
}


LinuxBuild{
    message(+++++++++++++++++++++++++++++++++++++++++++++++)
    message(LinuxBuild libs files)
    message(+++++++++++++++++++++++++++++++++++++++++++++++)
    LIBS += -L$$PWD/lib/linux64_14/ -lavutil \
     -lavcodec \
     -lswscale \
     -lavformat \
     -lavdevice \
     -lswresample \
     -lavfilter

    # 版本信息
    VERSION = 1.1.9
    # 图标
    #RC_ICONS = Resources/sensetime.png
    # 公司名称
    QMAKE_TARGET_COMPANY = "Shenzhen SenseTime Co.,Ltd"
    # 产品名称
    QMAKE_TARGET_PRODUCT = "SenseEngine DLL"
    # 版权信息
    QMAKE_TARGET_COPYRIGHT = "2018-2028,Shenzhen SenseTime Co.,Ltd"
}
