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
	, m_buffer(nullptr)
	, m_position(nullptr)
	, m_buf_size(0)
	, m_count(0)
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
	if(!m_cptr) return cycle_state::complete;

	feed_out(m_cptr, LineAudio);
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

//	onStateChange(PRESTREAM);
//	workState = PRESTREAM;
	log("Done");
	m_samples_played = 0;

}

void SuFlac::cache_chunk() {
	if(!m_buffer) return;

	{
		auto tmp = cache_alloc(1);
		m_cptr = tmp;
	}

	auto tmp = cache_alloc(1);
	auto csz = m_period_size;
	if(m_count < csz) csz = (m_count/2);

	tmp.copy_from(m_position, csz);
	routine::sound::deinterleave2(*tmp, *m_cptr, m_period_size);

	m_count -= csz*2;
	m_position += csz*2;
	//workState = STREAMING;
}

cycle_state SuFlac::resync() {
	m_period_size = global_profile().period;
	return cycle_state::complete;
}

void SuFlac::reset_buffer(unsigned int total_samples) {

	if(m_buf_size < total_samples) {

		if(m_buffer) delete[] m_buffer;

		m_buffer = new PcmSample[total_samples];
	}
	
	m_buf_size = total_samples;
	m_count = m_buf_size;
	m_position = m_buffer;
}
