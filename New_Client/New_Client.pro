#-------------------------------------------------
#
# Project created by QtCreator 2018-07-24T00:44:43
#
#-------------------------------------------------

QT       += core gui network axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = New_Client
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    client.cpp \
    logindialog.cpp \
    welcome.cpp \
    chpwd.cpp \
    hotel_detail.cpp \
    password.cpp \
    book.cpp \
    usertrades.cpp \
    hotel2.cpp \
    rooms.cpp \
    hoteltrades.cpp

HEADERS += \
        mainwindow.h \
    client.h \
    logindialog.h \
    welcome.h \
    chpwd.h \
    hotel_detail.h \
    password.h \
    book.h \
    usertrades.h \
    hotel2.h \
    rooms.h \
    hoteltrades.h

RESOURCES += \
    res.qrc

FORMS += \
    chpwd.ui
