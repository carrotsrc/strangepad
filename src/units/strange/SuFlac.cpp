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
{

	add_output("audio");
	register_midi_handler("pause",[this](midi::msg){
		trigger_cycle();
	});
}

SuFlac::~SuFlac() {
	if(m_buffer) delete[] m_buffer;
}

cycle_state SuFlac::cycle() {

	/* potential problem could be that
	 * the system is starved, so some
	 * sort of way of triggering a 
	 * recycle is necessary (low priority)
	 */
	if(!m_num_cached) return cycle_state::complete;

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
	return cycle_state::complete;
}

void SuFlac::load_file(std::string path) {
	std::stringstream ss;
	ss << "Loading " << path;
	log(ss.str());
	ss.str("");

	SndfileHandle file(path.c_str());

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


	log("Done");
	m_samples_played = 0;

}

void SuFlac::cache_chunk() {
	if(!m_buffer) return;

	auto tc = 5 - m_num_cached;

	for(auto i = 0; i < tc; i++) {

		auto inter = cache_alloc(1);
		auto deint = cache_alloc(1);

		auto csz = m_period_size * m_num_channels;

		if(m_remain < csz) csz = m_remain;
		inter.copy_from(m_position, csz);

		routine::sound::deinterleave2(*inter, *deint, m_period_size);

		m_remain -= csz*2;
		m_position += csz*2;
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
