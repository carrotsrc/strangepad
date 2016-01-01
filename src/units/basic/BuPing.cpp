#include <cmath>

#include "framework/routine/midi.hpp"
#include "BuPing.hpp"
using namespace siocom;

BuPing::BuPing(std::string label)
	: unit(siocom::unit_type::step, "BuPing", label)
	, m_fc(240)
	, m_x(0)

	, m_cycle(0)
	, m_prog(0)

	, m_ping_len(200)
	, m_2pi{M_PI*2}

	, m_in_ping(false)
	
{ 

	add_input("audio_in");
	add_output("audio");
	
	register_midi_handler("beat", [this](siomid::msg m) {
		if(m.v < 127) return;
		m_current = strangeio::routine::debug::clock_time();
		m_diff = strangeio::routine::debug::clock_delta_ms(m_last, m_current);
		m_last = m_current;
		m_bpm = static_cast<unsigned int>(60000/m_diff);
		m_bpm_centre = m_bpm;
		log("BPM: " + std::to_string(m_bpm));
	});
	
	register_midi_handler("ping", [this](siomid::msg m) {
		if(m.v < 127 || !m_bpm) return;
		auto s = m_state.load();
		
		if(s == working_state::passing) {

			m_cycle = (int)((float)global_profile().fs) * (60.0f / ((float)(m_bpm)));
			m_ping_smp = (global_profile().fs/1000.0) * m_ping_len;
			m_in_ping = true;
			
			m_x = 0;
			m_prog = 0;
			m_ping_num = 0;
			m_beat = 1;
			m_fc = 240;
			m_ping_num = 0;
			m_prog = 0;
			
			log("pinging at " + std::to_string(m_bpm) + "bpm");
			m_state.store(working_state::pinging);
			this->toggle_led(1);
			
		} else {
			
			m_state.store(working_state::passing);
			m_in_ping = false;
			this->toggle_led(0);
		}
	});
	
	register_midi_handler("modifier", [this](siomid::msg m) {
		m_bpm = m_bpm_centre + (m.v - 64);
		log("BPM: " + std::to_string(m_bpm));
	});
	
	register_midi_handler("centre", [this](siomid::msg m) {
		if(m.v == 0) return;
		m_bpm_centre = m_bpm;
		log("centred BPM: " + std::to_string(m_bpm_centre));
	});
	
	register_midi_handler("lock", [this](siomid::msg m) {
		if(m.v == 0) return;
		register_metric(profile_metric::bpm, (int)m_bpm);
		trigger_sync((sync_flag)sync_flags::glob_sync);
		log("locked BPM: " + std::to_string(m_bpm));
	});

	register_midi_handler("increment", [this](siomid::msg m) {
		if(m.v == 0) return;
		m_bpm++;
	});
	
	register_midi_handler("decrement", [this](siomid::msg m) {
		if(m.v == 0) return;
		m_bpm--;
	});

	register_midi_led("idle", 0);
	register_midi_led("pinging", 1);	
		
	
	m_last = m_current = siortn::debug::zero_timepoint();
	m_state = working_state::passing;
	m_modifier_on = 0;
	
}

BuPing::~BuPing() {
}

siocom::cycle_state BuPing::cycle() {
	auto state = m_state.load();
	if(state == working_state::pinging)
			write_ping();
			
	this->feed_out(m_ptr, 0);
	return siocom::cycle_state::complete;
}

void BuPing::feed_line(siomem::cache_ptr samples, int line) {
	m_ptr = samples;
}

siocom::cycle_state BuPing::init() {
	this->toggle_led(0);
	log("Initialised");
	return siocom::cycle_state::complete;
}

siocom::cycle_state BuPing::resync(siocom::sync_flag flags) {
	if(flags & (sync_flag)sync_flags::glob_sync) {
		if(global_profile().bpm != (signed int)m_bpm) {
			m_bpm_centre = m_bpm = global_profile().bpm;
		}
	}
	return siocom::cycle_state::complete;
}

void BuPing::write_ping() {
	
	auto profile = global_profile();
	
	auto sz = profile.period;
	auto fs = profile.fs;
	auto i = 0;
	auto c1 = 0;
	auto c2 = sz;
	for(i = 0; i < sz; i++) {
		if(m_prog == 0 && m_ping_num == 0) {
			log("Beat: " + std::to_string(m_beat));
		}
		
		if(m_in_ping) {
			auto value = sin(m_2pi * m_fc * m_x++ / fs)/3;
			m_ptr[c1++] += value;
			m_ptr[c2++] += value;
		}
		

		
		if(++m_prog == m_cycle) {
			m_prog = 0;
			m_x = 0;
			m_in_ping = true;
		} else if(m_prog == m_ping_smp) {
			m_beat++;
			if(++m_ping_num == 4) {
				m_fc = 240;
				m_ping_num = 0;
			} else {
				m_fc = 180;
			}
			m_in_ping = false;
		}
	}
	
	
}

UnitBuilder(BuPing);