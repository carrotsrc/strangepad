######################################################################
# Automatically generated by qmake (2.01a) Wed Jul 15 18:20:26 2015
######################################################################

TEMPLATE = lib
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
LIBS += -L$(RACKOONIOFW) -lrackio

DESTDIR=../../../bin/pads/
OBJECTS_DIR=../../../build/obj

QMAKE_CXXFLAGS_DEBUG += --std=c++11 -I$(RACKOONIOFW)
QMAKE_CXXFLAGS_RELEASE += --std=c++11 -I$(RACKOONIOFW)

# Input
HEADERS += SpSine.hpp
SOURCES += SpSine.cpp
