######################################################################
# Automatically generated by qmake (2.01a) Tue Jul 14 23:47:22 2015
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += . sys
INCLUDEPATH += .
DESTDIR=../../bin
OBJECTS_DIR=../../build/obj
MOC_DIR=../../build
LIBS += -L$(RACKOONIOFW) -lrackio -L../../bin/ -lui -lleveldb

QMAKE_CXXFLAGS_DEBUG += --std=c++11 -I$(RACKOONIOFW) -ggdb
QMAKE_CXXFLAGS_RELEASE += --std=c++11 -I$(RACKOONIOFW) -ggdb


# Input
SOURCES += sys/RigDesc.cpp sys/ConfigLoader.cpp
SOURCES += ui/SHud.cpp ui/SWindow.cpp sys/entry.cpp
