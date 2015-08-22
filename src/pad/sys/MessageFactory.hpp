#ifndef __MESSAGEFACTORY_HPP_1440270832__
#define __MESSAGEFACTORY_HPP_1440270832__
#include "framework/factories/GenericEventMessageFactory.h"
#include "../msg/ClientMessages.hpp"

class MessageFactory : public StrangeIO::GenericEventMessageFactory {
protected:
	std::unique_ptr<StrangeIO::EventMessage> clientMessage(StrangeIO::EventType type);
};
#endif

