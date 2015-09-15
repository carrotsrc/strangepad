TEMPLATE = lib
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . ../ ../../units
LIBS += -L$(STRANGEFW) -L../../../bin/ -lstrangeio ./units/SuDelay.rso

DESTDIR=../../../bin/pads/
OBJECTS_DIR=../../../build/obj

QMAKE_CXXFLAGS_DEBUG += --std=c++11 -I$(STRANGEFW)/framework/include --ggdb
QMAKE_CXXFLAGS_RELEASE += --std=c++11 -I$(STRANGEFW)/framework/include -ggdb

# Input
HEADERS += SpDelay.hpp ControllerPanel.hpp
SOURCES += SpDelay.cpp ControllerPanel.cpp
