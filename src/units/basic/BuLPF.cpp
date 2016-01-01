#include "BuLPF.hpp"
#include "framework/routine/midi.hpp"
using namespace siocom;
/*
#define CoA0 +0.0025f
#define CoB1 -0.9975f


#define CoA0 +0.8284f
#define CoB1 +0.1716f
*/

#define CoA0 +0.0141
#define CoB1 -0.9859
BuLPF::BuLPF(std::string label)
	: unit(siocom::unit_type::step, "BuLPF", label)
	, m_zb1c1(0.0f)
	, m_zb1c2(0.0f)
	, m_a0(CoA0)
	, m_b1(CoB1)
{ 
	add_input("audio_in");
	add_output("audio");
	register_midi_handler("b1", [this](siomid::msg m) {
		auto b1 = CoB1 * siortn::midi::normalise_velocity128(m.v);
		log(std::to_string(b1));
		m_b1 = b1;
	});
	
	register_midi_handler("a0", [this](siomid::msg m) {
		auto a0 = CoA0 * siortn::midi::normalise_velocity128(m.v);
		log(std::to_string(a0));
		m_a0 = a0;
	});
}

BuLPF::~BuLPF() {
}

siocom::cycle_state BuLPF::cycle() {
	if(!m_cache) return siocom::cycle_state::complete;
	auto sz = m_cache.block_size()/2;
	
	auto a0 = m_a0;
	auto b1 = m_b1;
	auto zb1c1 = m_zb1c1;
	auto zb1c2 = m_zb1c2;
	auto c1 = 0u, c2 = sz;
	
	for(auto i = 0u; i < sz; i++) {
		auto s1 = m_cache[c1] * a0;
		auto s2 = m_cache[c2] * a0;
		s1 -= zb1c1;
		s2 -= zb1c2;
		m_cache[c1++] = s1;
		m_cache[c2++] = s2;
		
		zb1c1 = s1 * b1;
		zb1c2 = s2 * b1;
	}
	
	m_zb1c1 = zb1c1;
	m_zb1c2 = zb1c2;
	this->feed_out(m_cache, 0);
	return siocom::cycle_state::complete;
}

void BuLPF::feed_line(siomem::cache_ptr samples, int line) {
	m_cache = samples;
	
}

siocom::cycle_state BuLPF::init() {
	log("Initialised");
	return siocom::cycle_state::complete;
}

siocom::cycle_state BuLPF::resync(siocom::sync_flag flags) {

	return siocom::cycle_state::complete;
}

UnitBuilder(BuLPF);



