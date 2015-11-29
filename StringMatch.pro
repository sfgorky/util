#-------------------------------------------------
#
# Project created by QtCreator 2015-10-18T20:41:39
#
#-------------------------------------------------

TARGET = match
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += STRING_MATCH_MAIN=1


SOURCES += \
    ./StringMatch.cpp


HEADERS += \
    ./StringMatch.h



CONFIG(debug, debug|release) {
    DESTDIR = ../build/debug
} else {
    DESTDIR = ../build/release
}

OBJECTS_DIR = $$DESTDIR/.obj 
MOC_DIR = $$DESTDIR/.moc 
RCC_DIR = $$DESTDIR/.qrc 
UI_DIR = $$DESTDIR/.ui


