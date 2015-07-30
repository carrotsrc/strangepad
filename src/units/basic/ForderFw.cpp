#include "ForderFw.hpp"
using namespace RackoonIO;

ForderFw::ForderFw()
RackUnit("ForderFw") {
	addJack("audio", SEQ_JACK);
	addPlug("audio_out");
	mWorkState = IDLE;
}

FeedState ForderFw::feed(Jack*) {

}

void ForderFw::setConfig(std::string,std::string) {

}

RackState ForderFw::init() {
	UnitMsg("Initialised");
	mWorkState = READY;
}

RackState ForderFw::cycle() {

}

void ForderFw::block(RackoonIO::Jack*) {

}
