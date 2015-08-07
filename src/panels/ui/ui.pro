######################################################################
# Automatically generated by qmake (2.01a) Wed Jul 15 19:35:38 2015
######################################################################

TEMPLATE = lib
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

LIBS += -L$(STRANGEFW) -lstrangeio

DESTDIR=../../../bin/
OBJECTS_DIR=../../../build/obj

QMAKE_CXXFLAGS_DEBUG += --std=c++11 -I$(STRANGEFW)
QMAKE_CXXFLAGS_RELEASE += --std=c++11 -I$(STRANGEFW)

# Input
HEADERS += SButton.hpp SWaveform.hpp SSlider.hpp SKnob.hpp SVIndicator.hpp SPad.hpp
SOURCES += SButton.cpp SWaveform.cpp SSlider.cpp SKnob.cpp SVIndicator.cpp SPad.cpp
