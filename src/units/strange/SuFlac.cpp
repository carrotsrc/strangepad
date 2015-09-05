#include "SuFlac.hpp"
using namespace strangeio::component;

SuFlac::SuFlac(std::string label)
	: unit(unit_type::mainline, "SuFlac", label)
{

}

SuFlac::~SuFlac() {

}

strangeio::component::cycle_state SuFlac::cycle() {

	return cycle_state::complete;
}
void SuFlac::feed_line(strangeio::memory::cache_ptr samples, int line) {

}
strangeio::component::cycle_state SuFlac::init() {
	return cycle_state::complete;
}
