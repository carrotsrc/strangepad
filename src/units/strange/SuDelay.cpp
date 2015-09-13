#include "SuDelay.hpp"
#include <sstream>
#define SuDelayAudio 0

SuDelay::SuDelay(std::string label)
	: unit(siocom::unit_type::step, "SuDelay", label)

	, m_delay_time(500)
	, m_delay_size(0)
	, m_period_size(0)
	, m_spms(0)
	, m_a(0.5)

	, m_delay_buffer(nullptr)
	, m_write_r(nullptr), m_read_r(nullptr), m_end_r(nullptr)
	, m_write_l(nullptr), m_read_l(nullptr), m_end_l(nullptr)
	, m_start_r(nullptr), m_start_l(nullptr)

{
	add_input("audio_in");
	add_output("audio");
	m_ws = working_state::passing;

	register_midi_handler("toggle", [this](siomid::msg m) {
		if(m.v < 127) return;

		if(m_ws == working_state::ready) action_start();
		else if(m_ws == working_state::filtering) action_stop();
	});
}

SuDelay::~SuDelay() {
}

siocom::cycle_state SuDelay::cycle() {
	if(m_cptr == false) {
		return siocom::cycle_state::complete;
	}

	if(m_ws == working_state::passing) {
		feed_out(m_cptr, SuDelayAudio);
		return siocom::cycle_state::complete;
	}

	if(m_ws == working_state::filtering) {
		auto c1 = 0u, c2 = m_period_size;
		for(auto i = 0u; i < m_period_size; i++) {

			auto sl = (*m_write_l * m_a) + m_cptr[c1];
			m_cptr[c1++] = sl;
			*m_write_l = sl;

			auto sr = (*m_write_r * m_a) + m_cptr[c2];
			m_cptr[c2++] = sr;
			*m_write_r = sr;

			if(++m_write_l == m_end_l) m_write_l = m_start_l;
			if(++m_write_r == m_end_r) m_write_r = m_start_r;
		}
		feed_out(m_cptr, SuDelayAudio);
		return siocom::cycle_state::complete;
	}

	if(m_ws == working_state::priming || m_ws == working_state::ready) {
		std::stringstream ss;
		auto rem = m_period_size;
		auto wrap = m_end_l - m_write_l;


		if( wrap < m_period_size) {

			std::copy(m_cptr.get(), m_cptr.get()+wrap, m_write_l);
			std::copy(m_cptr.get()+m_period_size, m_cptr.get() + m_period_size + wrap, m_write_r);

			m_write_l = m_start_l;
			m_write_r = m_start_r;

			rem -= wrap;
			if(m_ws == priming) {
				m_ws = working_state::ready;
				log("Primed");
			}

		}

		std::copy(m_cptr.get(), m_cptr.get()+wrap, m_write_l);
		std::copy(m_cptr.get()+m_period_size, m_cptr.get() + m_period_size + wrap, m_write_r);
		m_write_l += rem;
		m_write_r += rem;
	}
	feed_out(m_cptr, SuDelayAudio);
	return siocom::cycle_state::complete;
}

void SuDelay::listen_onchange(std::weak_ptr<std::function<void(SuDelay::working_state)> > cb) {
	m_onchange_listeners.push_back(cb);
}

void SuDelay::feed_line(siomem::cache_ptr samples, int line) {
	m_cptr = samples;
}

siocom::cycle_state SuDelay::resync(siocom::sync_flag flags) {
	if( flags & (siocom::sync_flag)siocom::sync_flags::glob_sync) {
		if(m_delay_buffer == nullptr) {
			reset_delay();
		}
	}
	return siocom::cycle_state::complete;
}


siocom::cycle_state SuDelay::init() {
	log("Initialised");
	return siocom::cycle_state::complete;
}

void SuDelay::reset_delay() {

	auto profile = global_profile();

	m_spms = profile.fs / 1000u;
	m_period_size = profile.period;

	auto sz = (unsigned int) ((m_spms * m_delay_time)*2);
	if(sz > m_delay_size) {

		if(m_delay_buffer != nullptr) {
			delete[] m_delay_buffer;
		}

		m_delay_buffer = new PcmSample[sz];

	}
	m_delay_size = sz;

/*	
	std::stringstream ss;
	ss << "Create delay buffer of " << m_delay_size << " samples";
	log(ss.str());
*/

	auto chan_sw = (m_delay_size/2);
	m_start_l = m_write_l = m_read_l = m_delay_buffer;
	m_end_l = m_delay_buffer + chan_sw;

	m_start_r = m_write_r = m_read_r = (m_delay_buffer+chan_sw);
	m_end_r = m_start_r + chan_sw;

	m_ws = working_state::priming;

}

void SuDelay::event_onchange(SuDelay::working_state state) {
	m_ws = state;
	for(auto wptr : m_onchange_listeners) {
		if(auto cb = wptr.lock()) {
			(*cb)(state);
		}
	}
}

void SuDelay::action_start() {
	if(m_ws == working_state::passing || m_ws == working_state::passing) return;

	m_ws = working_state::filtering;
	log("Echo delay on");
}

void SuDelay::action_stop() {
	if(m_ws == working_state::passing || m_ws == working_state::passing) return;

	m_ws = working_state::ready;
	log("Echo delay off");
}

UnitBuilder(SuDelay);