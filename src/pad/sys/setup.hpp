#ifndef __SETUP_HPP_1438172115__
#define __SETUP_HPP_1438172115__

#include <QStack>
#include <QVector>

#include "framework/rack/Rack.h"
#include "ui/SHud.hpp"
#include "ConfigLoader.hpp"
#include "PadLoader.hpp"

void setupRackoon(StrangeIO::Rack *rack);
QVector<SHud*> setupRig(const RigDesc & rig, PadLoader *padLoader, StrangeIO::Rack *rack);
#endif
