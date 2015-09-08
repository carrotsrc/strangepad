#include "SuMixer.hpp"
#include "framework/routine/midi.hpp"
#define ChannelA 0
#define ChannelB 1
#define AudioOut 0
SuMixer::SuMixer(std::string label)
	: siospc::combine("SuMixer", label)
	, m_gain_master(0.5)
	, m_gain_chan_a(1.0)
	, m_gain_chan_b(1.0)
{
	add_input("channel_a");
	add_input("channel_b");
	add_output("audio");
	// since this is a combine
	// setup line states
	init_input_states();

	register_midi_handler("master_gain",[this](siomid::msg m) {
		action_gain_master(m.v);
	});

	register_midi_handler("channel_a_gain",[this](siomid::msg m) {
		action_gain_chan_a(m.v);
	});

	register_midi_handler("channel_b_gain",[this](siomid::msg m) {
		action_gain_chan_b(m.v);
	});
}

SuMixer::~SuMixer() {
	
}

void SuMixer::feed_line(siomem::cache_ptr samples, int line) {
	switch(line) {
	case ChannelA: m_chan_a = samples; break;
	case ChannelB: m_chan_b = samples; break;
	}
}

siocom::cycle_state SuMixer::cycle() {
	if(all_active()) {
		if(!m_chan_a || !m_chan_b) {
			return siocom::cycle_state::partial;
		}
		mix_channels();

	} else {
		single_channel();
	}
	return siocom::cycle_state::complete;
}

void SuMixer::mix_channels() {
		auto total = m_chan_a.block_size();
		for(auto i = 0u; i < total; i++) { 
			auto sampleA = m_chan_a[i] * m_gain_final_a;
			auto sampleB = m_chan_b[i] * m_gain_final_b;
			
			m_chan_a[i] = sampleA + sampleB;
		}
		feed_out(m_chan_a, AudioOut);
}

void SuMixer::single_channel() {
	if(m_chan_a) {

		auto total = m_chan_a.block_size();

		for(auto i = 0u; i < total; i++) {
			auto sample = m_chan_a[i] * m_gain_final_a;
			m_chan_a[i] = sample;
		}
		feed_out(m_chan_a, AudioOut);

	} else if(m_chan_b) {

		auto total = m_chan_b.block_size();
		for(auto i = 0u; i < total; i++) {
			auto sample = m_chan_b[i] * m_gain_final_b;
			m_chan_b[i] = sample;
		}
		feed_out(m_chan_b, AudioOut);
	}
}


siocom::cycle_state SuMixer::init() {
	m_gain_final_a = m_gain_chan_a * m_gain_master;
	m_gain_final_b = m_gain_chan_b * m_gain_master;
	log("Initialised");
	return siocom::cycle_state::complete;
}
void SuMixer::action_gain_master(int value) {
	auto fv = siortn::midi::normalise_velocity128(value);
	m_gain_master = fv;
	m_gain_final_a = m_gain_chan_a * m_gain_master;
	m_gain_final_b = m_gain_chan_b * m_gain_master;

}

void SuMixer::action_gain_chan_a(int value) {
	auto fv = siortn::midi::normalise_velocity128(value);
	m_gain_chan_a = fv;
	m_gain_final_a = m_gain_chan_a * m_gain_master;
}

void SuMixer::action_gain_chan_b(int value) {
	auto fv = siortn::midi::normalise_velocity128(value);
	m_gain_chan_b = fv;
	m_gain_final_b = m_gain_chan_b * m_gain_master;
}
UnitBuilder(SuMixer);
