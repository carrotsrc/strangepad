######################################################################
# Automatically generated by qmake (2.01a) Wed Jul 15 18:20:26 2015
######################################################################

TEMPLATE = lib
TARGET = SpSine
DEPENDPATH += .
INCLUDEPATH += . ../
LIBS += -L$(STRANGEFW) -L../../../bin/ -lstrangeio -lui

DESTDIR=../../../bin/pads/
OBJECTS_DIR=../../../build/obj

QMAKE_CXXFLAGS_DEBUG += --std=c++14 -I$(STRANGEFW)
QMAKE_CXXFLAGS_RELEASE += --std=c++14 -I$(STRANGEFW)

# Input
HEADERS += SpSine.hpp PanelOverview.hpp
SOURCES += SpSine.cpp PanelOverview.cpp
