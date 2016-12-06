#-------------------------------------------------
#
# Project created by QtCreator 2016-11-29T09:49:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NxQuickRearCam
TEMPLATE = app

# Add RearCam Library
INCLUDEPATH += ../librearcam/include
LIBS += -L../librearcam/lib -lnxrearcam

SOURCES += main.cpp\
		mainwindow.cpp \
		eventsender.cpp

HEADERS  += mainwindow.h \
			eventsender.h

FORMS    += mainwindow.ui
