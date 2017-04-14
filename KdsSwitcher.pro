#-------------------------------------------------
#
# Project created by QtCreator 2015-08-07T14:03:18
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KdsSwitcher
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    pointedit.cpp \
    kds.cpp

HEADERS  += mainwindow.h \
    pointedit.h \
    kds.h

FORMS    += mainwindow.ui \
    pointedit.ui

RC_ICONS = icon.ico

DISTFILES += \
    KdsSwitcher_resource.rc \
    myStyle.qss
