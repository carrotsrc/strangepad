######################################################################
# Automatically generated by qmake (2.01a) Wed Jul 15 18:20:26 2015
######################################################################

TEMPLATE = lib
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . ../ ../../units
LIBS += -L$(STRANGEFW) -L../../../bin/ -lstrangeio ./units/SuFlac.rso

DESTDIR=../../../bin/pads/
OBJECTS_DIR=../../../build/obj

QMAKE_CXXFLAGS_DEBUG += --std=c++11 -I$(STRANGEFW)/framework/include --ggdb
QMAKE_CXXFLAGS_RELEASE += --std=c++11 -I$(STRANGEFW)/framework/include -ggdb

# Input
HEADERS += SpFlac.hpp PanelWaveview.hpp
SOURCES += SpFlac.cpp PanelWaveview.cpp
