#-------------------------------------------------
#
# Project created by QtCreator 2014-09-08T11:22:06
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = txtToDic
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    txtToDic.cpp \
    ADThreadPool.cpp \
    indexer.cpp \
    DictionaryIndex.cpp \
    PairStorage.cpp \
    Merger.cpp \
    Optimiser.cpp

HEADERS += \
    txtToDic.h \
    indexer.h \
    DictionaryIndex.h \
    PairStorage.h \
    Merger.h \
    Optimiser.h
