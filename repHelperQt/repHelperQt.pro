#-------------------------------------------------
#
# Project created by QtCreator 2018-06-24T11:36:05
#
#-------------------------------------------------

#DEFINES += VSDLL
DEFINES += QTDLL

DEFINES += REPHELPERQT_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG(release, debug|release): {
    DEFINES += F_NO_DEBUG
}

CONFIG(debug, debug|release): {

}

TEMPLATE = lib
contains(DEFINES, VSDLL){

TARGET = repHelper
SOURCES += \
        rephelper.cpp \
        stdafx.cpp \
        dllmain.cpp

HEADERS += \
        stdafx.h \
        targetver.h

} else {

QT += widgets
TARGET = repHelperQt
SOURCES += \
        qmfcapp.cpp \
        dllmainqt.cpp \
        rephelperqt.cpp

HEADERS += \
        qmfcapp.h \
        rephelperqt.h \
        rephelperqt_p.h \
        rephelperqt_global.h


}



