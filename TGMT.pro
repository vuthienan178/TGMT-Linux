QT += core
QT -= gui

CONFIG += c++11

TARGET = TGMT
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    utility.cpp \
    cutimage.cpp \
    image.cpp \
    stdafx.cpp \
    mapping.cpp
INCLUDEPATH += /usr/local/include/opencv /usr/include/tesseract
LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc
LIBS += -L/usr/lib -ltesseract
HEADERS += \
    utility.h \
    handlingdefine.h \
    image.h \
    stdafx.h
