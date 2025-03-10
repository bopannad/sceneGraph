QT += testlib

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

# Include the source code to test
INCLUDEPATH += ../../
SOURCES += ../../customnavlogger.cpp \
           tst_customnavlogger.cpp

HEADERS += ../../customnavlogger.h

# Define the path to find resources if needed
DEFINES += SRCDIR=\\\"$$PWD/\\\"