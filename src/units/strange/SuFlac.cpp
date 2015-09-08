#include <sstream>
#include <sndfile.hh>

#include "SuFlac.hpp"
#include "framework/routine/sound.hpp"

using namespace strangeio;
using namespace strangeio::component;
#define CHUNK_SIZE 0x100000
#define LineAudio 0

SuFlac::SuFlac(std::string label)
	: unit(unit_type::mainline, "SuFlac", label)
	, m_num_cached(0)
	, m_rindex(0)
	, m_windex(0)
	, m_buffer(nullptr)
	, m_position(nullptr)
	, m_buf_size(0)
	, m_remain(0)
	, m_samples_played(0)
	, m_flac_path("")
{

	add_output("audio");
	register_midi_handler("pause",[this](midi::msg m){
		if(m.v != 127) return;
		action_start_stream();
	});

	register_midi_handler("load",[this](midi::msg m){
		if(m.v != 127) return;
		log("Starting load");
		load_file();
	});
}

SuFlac::~SuFlac() {
	if(m_buffer) delete[] m_buffer;
}

cycle_state SuFlac::cycle() {
	/* potential problem could be that
	 * the system is starved, so some
	 * sort of way of triggering a 
	 * recycle is necessary (low priority).
	 * 
	 * For now, the cached periods should
	 * be OK.
	 */
	if(!m_num_cached) {
		return cycle_state::error;
	}

	feed_out(m_cptr[m_rindex++], LineAudio);
	m_num_cached--;
	if(m_rindex == 5) m_rindex = 0;

	add_task(std::bind(&SuFlac::cache_chunk, this));
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
	}

	m_position = m_buffer;
	m_num_cached = 0;

	add_task(std::bind(&SuFlac::cache_chunk, this));
	log("Done");
	event_onchange(SuFlac::prestream);
	m_samples_played = 0;
	
}

void SuFlac::cache_chunk() {
	
	if(!m_buffer) return;

	auto tc = 5 - m_num_cached;
	
	for(auto i = 0; i < tc; i++) {
		if(m_remain == 0) return;

		auto samples = cache_alloc(1);
		auto deint = cache_alloc(1);

		auto csz = m_period_size * m_num_channels;

		if(m_remain < csz) csz = m_remain;
		samples.copy_from(m_position, csz);

		routine::sound::deinterleave2(*samples, *deint, m_period_size);

		m_remain -= csz;
		m_position += csz;
		m_cptr[m_windex] = deint;

		m_num_cached++;
		if(++m_windex == 5) m_windex = 0;
	}

}

cycle_state SuFlac::resync() {
	m_period_size = global_profile().period;
	m_num_channels = global_profile().channels;
	return cycle_state::complete;
}

void SuFlac::reset_buffer(unsigned int total_samples) {

	if(m_buf_size < total_samples) {

		if(m_buffer) delete[] m_buffer;

		m_buffer = new PcmSample[total_samples];
	}
	m_num_cached = m_windex = m_rindex = 0;

	m_buf_size = total_samples;
	m_remain = total_samples;
	m_position = m_buffer;
}

void SuFlac::set_configuration(std::string key, std::string value) {
	if(key == "flac") {
		m_flac_path = value;
	} else if(key == "kick_start") {
		trigger_cycle();
	}
}

void SuFlac::event_onchange(SuFlac::working_state state) {
	for(auto wptr : m_onchange_listeners) {
		if(auto cb = wptr.lock()) {
			(*cb)(state);
		}
	}
}


void SuFlac::action_load_file(std::string path) {
	m_flac_path = path;
	load_file();
}

void SuFlac::action_start_stream() {
	if(!m_buffer || m_buf_size == 0) return;

	if(unit_profile().state == (int)line_state::inactive) {
		register_metric(profile_metric::state, (int)line_state::active);
		trigger_sync((sync_flag)sync_flags::upstream);

		if(global_profile().state == (int)line_state::inactive) {
			trigger_cycle();
		}
	}
}

std::string SuFlac::probe_flac_path() const {
	return m_flac_path;
}

const PcmSample* SuFlac::probe_flac_data() const {
	return m_buffer;
}



void SuFlac::listen_onchange(std::weak_ptr<std::function<void(SuFlac::working_state)> > cb) {
	m_onchange_listeners.push_back(cb);
}

unsigned int SuFlac::probe_total_spc() const {
	return m_buf_size  / unit_profile().channels;
}

UnitBuilder(SuFlac);
