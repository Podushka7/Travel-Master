QT += core widgets testlib

CONFIG += c++11 console
CONFIG -= app_bundle

HEADERS += \
    tour.h \
    tourdatabase.h

SOURCES += \
    tour.cpp \
    tourdatabase.cpp \
    database_test.cpp
