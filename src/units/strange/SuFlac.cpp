#include <sstream>
#include <sndfile.hh>
#include <thread>
#include <iostream>
#include <algorithm>
#include "SuFlac.hpp"
#include "framework/routine/sound.hpp"
#include "framework/component/debug.hpp"

using namespace strangeio;
using namespace strangeio::component;
#define CHUNK_SIZE 0x10000
#define LineAudio 0

#define LED_READY 0
#define LED_STREAMING 1
#define LED_PAUSED 1

enum class led_state {
	ready, streaming, paused
};

SuFlac::SuFlac(std::string label)
	: siospc::mainline("SuFlac", label)
	, m_cbuf(SuFlacCacheSize)
	, m_position_history(SuFlacCacheSize)
	, m_ph_start(0), m_ph_end(0)
	, m_buffer(nullptr)
	, m_position(nullptr)
	, m_buf_size(0)
	, m_remain(0)
	, m_old_period(0)
	, m_track_bpm(0)
	, m_samples_played(0)
	, m_jump(false)
	, m_final(false)
	, m_flac_path("")
	, m_bpm_sync(false)

{

	add_output("audio");
	register_midi_handler("pause",[this](midi::msg m){
		if(m.v != 127) return;
		action_start_stream();
	});

	register_midi_handler("load",[this](midi::msg m){
		if(m.v != 127) return;
		load_file();
	});

	register_midi_led("ready", (int)led_state::ready);
	register_midi_led("streaming", (int)led_state::streaming);
	this->m_ws = working_state::idle;
	
}

SuFlac::~SuFlac() {
	if(m_buffer) delete[] m_buffer;
}

cycle_state SuFlac::cycle() {
	if(m_ws != working_state::streaming && m_ws != working_state::sync_paused) {
		return cycle_state::complete;
	}
	
	{
		std::lock_guard<std::mutex> lkg(m_buffer_mutex);
		
		if(!m_cbuf.size())
			return cycle_state::complete;
		
		feed_out(std::move(m_cbuf.move_front()), LineAudio);
		m_position_history.pop_front();
	}

	cache_task();
	if(m_remain) {
		m_samples_played += (m_period_size*2);
	}
	return cycle_state::complete;
}

void SuFlac::feed_line(strangeio::memory::cache_ptr samples, int line) {

}

cycle_state SuFlac::init() {
	register_metric(profile_metric::channels, 2); // default

	log("Initialised");
	return cycle_state::complete;
}

void SuFlac::load_file() {

	if(m_ws == working_state::streaming) return;

	std::lock_guard<std::mutex> lkg(m_buffer_mutex);

	clear_cache();

	std::stringstream ss;
	ss << "Loading " << m_flac_path;
	log(ss.str());
	ss.str("");

	event_onchange(SuFlac::loading);

	SndfileHandle file(m_flac_path.c_str());

	if(file.error() > 0) {
		ss << "Error occured when loading file - " << file.error();
		log(ss.str());
		return;
	}

	auto num_channels = file.channels();
	register_metric(profile_metric::channels, num_channels);

	// Two channels, two datapoints per frame
	auto total_samples = file.frames()*num_channels;

	reset_buffer(total_samples);

	// Load the whole file into memory (might stream it in future)
	while(file.read((PcmSample*)m_position, CHUNK_SIZE) == CHUNK_SIZE) {
		m_position += CHUNK_SIZE;
		std::this_thread::sleep_for(std::chrono::microseconds(15));
	}

	m_position = m_buffer;
	cache_chunk();

	log("Done");
	event_onchange(SuFlac::prestream);
	toggle_led((int)led_state::ready);
	
	/* The first period stays in the buffer
	 * until the second period is received
	 */
	m_samples_played = -m_period_size;
	m_final = false;
	
}

void SuFlac::cache_task() {
	std::lock_guard<std::mutex> lkg(m_buffer_mutex);
	cache_chunk();
}
void SuFlac::cache_chunk() {
	
	if(!m_buffer) return;

	auto p = m_period_size * m_num_channels;
	auto spaces = m_cbuf.capacity() - m_cbuf.size();
	
	for(auto i = 0u; i < spaces; i++) {
		if(m_remain == 0) {
			auto cptr = cache_alloc(1);
			std::fill_n(*cptr, *cptr+p, 0.0000001f);
			m_cbuf.push_back(std::move(cptr));
			continue;
		}

		auto samples = cache_alloc(1);
		auto out = cache_alloc(1);
		auto t = p;
		if(m_remain < t) {
			t = m_remain;
			m_final = true; // toggle final load
		}
		samples.copy_from(m_position, t);
		m_position_history.push_back(m_position);
		
		routine::sound::deinterleave2(std::move(samples), *out, m_period_size);
		m_cbuf.push_back(std::move(out));

		m_remain -= t;
		m_position += t;	
	}
}

cycle_state SuFlac::resync(siocom::sync_flag flags) {
	
	if(flags & (sync_flag)sync_flags::glob_sync) {
		m_num_channels = global_profile().channels;
		m_old_period = global_profile().period;
		auto bpm = global_profile().bpm;
		if(m_track_bpm != bpm) {
			m_track_bpm = bpm;
			event_onchange(working_state::bpm_update);
		}

	} else if(flags & (sync_flag)sync_flags::upstream) {

		if(m_ws == working_state::sync_streaming) {
			toggle_led((int)led_state::streaming);
			event_onchange(working_state::streaming);
			log("Streaming");
		} else if (m_ws == working_state::sync_paused) {
			event_onchange(working_state::paused);
			toggle_led((int)led_state::ready);
			log("Paused");
		}

	} else {
		auto lp = line_profile();
		m_period_size  = lp.period;
		if(lp.drift != m_old_drift) {
			
			
			{ // lock scope
				std::lock_guard<std::mutex> lkg(m_buffer_mutex);	
				
				m_old_drift = lp.drift;
				m_track_bpm_live = m_track_bpm + (m_track_bpm*lp.drift);

				if( m_period_size != m_old_period  && m_cbuf.size()) {
					m_old_period = m_period_size;
					reset_cache();
					cache_chunk();
				}
			} // ~lock scope
			
			add_task([this](){
				event_onchange(working_state::bpm_update);
			});

		}	
	}
	return cycle_state::complete;
}

void SuFlac::reset_buffer(unsigned int total_samples) {

	if(m_buf_size < total_samples) {

		if(m_buffer) delete[] m_buffer;

		m_buffer = new PcmSample[total_samples];
	}

	clear_cache();

	m_buf_size = total_samples;
	m_remain = total_samples;
	m_position = m_buffer;
}

void SuFlac::set_configuration(std::string key, std::string value) {
	if(key == "flac") {
		m_flac_path = value;
	} else if(key == "bpm_sync" && value == "true"){
		m_bpm_sync = true;
	}
}

void SuFlac::event_onchange(SuFlac::working_state state) {
	if(state != working_state::bpm_update)
		m_ws = state;

	for(auto wptr : m_onchange_listeners) {
		if(auto cb = wptr.lock()) {
			(*cb)(state);
		}
	}
}

void SuFlac::action_load_file(std::string path) {
	m_flac_path = path;
	add_task(std::bind(&SuFlac::load_file, this));
}

void SuFlac::action_start_stream() {
	if(!m_buffer || m_buf_size == 0) return;

	if(unit_profile().state == (int)line_state::inactive) {
		register_metric(profile_metric::state, (int)line_state::active);
		event_onchange(working_state::sync_streaming);
		trigger_sync((sync_flag)sync_flags::upstream);

		if(global_profile().state == (int)line_state::inactive) {
			trigger_cycle(); // need to kick start the process
		}
	} else {
		register_metric(profile_metric::state, (int)line_state::inactive);
		event_onchange(working_state::sync_paused);
		trigger_sync((sync_flag)sync_flags::upstream);
	}
}

std::string SuFlac::probe_flac_path() const {
	return m_flac_path;
}

const PcmSample* SuFlac::probe_flac_data() const {
	return m_buffer;
}

int SuFlac::probe_bpm() const {
	return m_track_bpm;
}

float SuFlac::probe_bpm_live() const {
	return m_track_bpm_live;
}


void SuFlac::listen_onchange(std::weak_ptr<std::function<void(SuFlac::working_state)> > cb) {
	m_onchange_listeners.push_back(cb);
}

unsigned int SuFlac::probe_total_spc() const {
	return m_buf_size  / unit_profile().channels;
}

signed int SuFlac::probe_progress() const {
	if(m_samples_played < 0) return 0;

	return m_samples_played;
}

void SuFlac::action_jump_to_sample(int sample) {
	std::lock_guard<std::mutex> lkg(m_buffer_mutex);
	auto check = m_ws;
	if(check != working_state::prestream
	&& check != working_state::paused) {
		return;
	}
	clear_cache();
	auto offset = sample;
	m_position = m_buffer + offset;
	m_remain = m_buf_size - offset;
	m_samples_played = offset;
	cache_task();
}

void SuFlac::set_bpm(int bpm) {
	log("zero - " + std::to_string(bpm) +"bpm");
	m_track_bpm_live = m_track_bpm = bpm;
	register_metric(profile_metric::bpm, m_track_bpm);
	if(m_bpm_sync) {
		trigger_sync((sync_flag)sync_flags::glob_sync);
	}
}

void SuFlac::clear_cache() {
	auto m = m_cbuf.size();
	for(auto i = 0u; i < m; i++) {
		auto c = m_cbuf.move_front();
		c.free();
	}
	
	m_cbuf.clear();
}

void SuFlac::reset_cache() {
	
	
	m_position = m_position_history.front();
	m_remain = (m_buffer + m_buf_size) - m_position;
	m_position_history.clear();
	clear_cache();
}

UnitBuilder(SuFlac);
