TEMPLATE = lib
TARGET = SpDelay
DEPENDPATH += .
INCLUDEPATH += . ../ ../../units
LIBS += -L$(STRANGEFW) -L../../../bin/ -lstrangeio ./units/SuDelay.rso
QT += widgets

DESTDIR=../../../bin/pads/
OBJECTS_DIR=../../../build/obj

QMAKE_CXXFLAGS_DEBUG += --std=c++14 -I$(STRANGEFW)/framework/include --ggdb
QMAKE_CXXFLAGS_RELEASE += --std=c++14 -I$(STRANGEFW)/framework/include -ggdb

# Input
HEADERS += SpDelay.hpp ControllerPanel.hpp
SOURCES += SpDelay.cpp ControllerPanel.cpp
