#-------------------------------------------------
#
# Project created by QtCreator 2018-11-27T14:26:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cameraDemo
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        camerawindow.cpp \
    caliform.cpp \
    doublecali.cpp \
    processui.cpp

HEADERS += \
        camerawindow.h \
    caliform.h \
    doublecali.h \
    processui.h

FORMS += \
        camerawindow.ui \
    caliform.ui \
    doublecali.ui \
    processui.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
INCLUDEPATH+=D:/opencv/opencv/build/QT/QTLib/include/opencv\
                    D:/opencv/opencv/build/QT/QTLib/include/opencv2\
                    D:/opencv/opencv/build/QT/QTLib/include
LIBS+=D:/opencv/opencv/build/QT/lib/libopencv_*

RESOURCES += \
    myimage.qrc
