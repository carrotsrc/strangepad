#include "SuDelay.hpp"

SuDelay::SuDelay(std::string label)
	:unit(siocom::unit_type::step, "SuDelay", label)

{

}

SuDelay::~SuDelay() {
}

siocom::cycle_state SuDelay::cycle() {
	return siocom::cycle_state::complete;
}

void SuDelay::listen_onchange(std::weak_ptr<std::function<void(SuDelay::working_state)> > cb) {
	m_onchange_listeners.push_back(cb);
}
void SuDelay::feed_line(siomem::cache_ptr samples, int line) {
}


siocom::cycle_state SuDelay::init() {
	return siocom::cycle_state::complete;
}
