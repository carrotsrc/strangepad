#include "MessageFactory.hpp"
using namespace StrangeIO;
std::unique_ptr<EventMessage> MessageFactory::clientMessage(EventType type) {
	std::unique_ptr<EventMessage> msg;
	switch(type) {
	case SndSamplesOut:
		msg = std::unique_ptr<EventMessage>(new SndSamplesOutMsg()); break;
	default:
		msg = nullptr; break;
	}

	msg->msgType = type;
	return std::move(msg);
}
