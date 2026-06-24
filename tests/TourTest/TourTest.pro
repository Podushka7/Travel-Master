QT += testlib
#QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$PWD/../Otchet

HEADERS += \
$$PWD/../Otchet/tour.h\
$$PWD/../Otchet/tourdatabase.h

SOURCES +=  \
tst_tourtest.cpp\
$$PWD/../Otchet/tour.cpp\
$$PWD/../Otchet/tourdatabase.cpp
