#include <thread>

#include "BuDump.hpp"
#include "framework/routine/sound.hpp"
#include "framework/routine/debug.hpp"
using namespace strangeio::component;
using namespace strangeio::spec;

BuDump::BuDump(std::string label)
	: dispatch("BuDump", label)
	, m_output("dump")
	, m_delay_us(5800)
{ 
	add_input("audio_in");
}

BuDump::~BuDump() {
}
#include <iostream>
siocom::cycle_state BuDump::cycle() {
	if(!m_cptr) return siocom::cycle_state::complete;

	
	auto cptr = cache_alloc(1);
	siortn::sound::interleave2(*m_cptr, *cptr, m_frames);
	m_cptr.free();

	for(auto i = 0u; i < m_samples; i++) {
		m_of.write(reinterpret_cast<char*>(&cptr[i]), sizeof(PcmSample));
	}

	
	this->add_task([this]() {
		std::this_thread::sleep_for(siortn::debug::us(m_delay_us));
		this->trigger_cycle();
	});
	
	return siocom::cycle_state::complete;
}

void BuDump::feed_line(siomem::cache_ptr samples, int line) {
	m_cptr = std::move(samples);
}

siocom::cycle_state BuDump::init() {
	log("Initialised");
	
	auto suffix = 0;
	std::string tmp;
	do {
		tmp = m_output + std::to_string(suffix++) + ".raw";
	} while(exists(tmp));
	
	m_filename = tmp;
	m_of.open(m_filename, std::ios_base::binary);
	log("dumping to " + m_filename);
	log("delay at " + std::to_string(m_delay_us) + "us" );	
	
	register_metric(profile_metric::fs, 44100);
	register_metric(profile_metric::period, 512);
	m_frames = 512;
	m_samples = 512*2;
	return siocom::cycle_state::complete;
}

bool BuDump::exists(std::string out) {
	std::ifstream f(out);
	return f.good();
}

siocom::cycle_state BuDump::resync(siocom::sync_flag flags) {
	return siocom::cycle_state::complete;
}


void BuDump::set_configuration(std::string key, std::string value) {
	if(key == "output") {
		m_output = value;
	} else if(key == "delay_us") {
		m_delay_us = std::stoi(value);
	}
}

UnitBuilder(BuDump);



