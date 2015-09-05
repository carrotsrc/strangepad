#include <sstream>
#include <sndfile.hh>

#include "SuFlac.hpp"

using namespace strangeio;
using namespace strangeio::component;
#define CHUNK_SIZE 0x100000
SuFlac::SuFlac(std::string label)
	: unit(unit_type::mainline, "SuFlac", label)
{
	add_output("audio");
	register_midi_handler("pause",[this](midi::msg){
		
	});
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

void SuFlac::load_file(std::string path) {
	log(path);
	SndfileHandle file(path.c_str());

	if(file.error() > 0) {
		std::stringstream ss;
		ss << "Error occured when loading file `" << path << "` with error " << file.error();
		log(ss.str());
		return;
	}


	// Two channels, two datapoints per frame
	m_buf_size = file.frames()*2;
	m_count = m_buf_size;

	if(m_buffer != nullptr)
		delete[] m_buffer;

	m_buffer = new PcmSample[m_buf_size];
	m_position = m_buffer;

	while(file.read((PcmSample*)m_position, CHUNK_SIZE) == CHUNK_SIZE) {
		m_position += CHUNK_SIZE;
	}

	m_position = m_buffer;

//	onStateChange(PRESTREAM);
//	workState = PRESTREAM;
	log("Initialised");
	m_samples_played = 0;

}
