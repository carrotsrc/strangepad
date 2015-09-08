#ifndef __SETUP_HPP_1438172115__
#define __SETUP_HPP_1438172115__

#include <QStack>
#include <QVector>

#include "framework/alias.hpp"
#include "framework/config/assembler.hpp"
#include "framework/component/rack.hpp"


#include "ui/SHud.hpp"
#include "ConfigLoader.hpp"
#include "PadLoader.hpp"


QVector<SHud*> setupRig(const RigDesc & rig, PadLoader *padLoader, siocom::rack *sys);
#endif
