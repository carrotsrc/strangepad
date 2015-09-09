#include "SuMixer.hpp"
#include "framework/routine/midi.hpp"
#define ChannelA 0
#define ChannelB 1
#define AudioOut 0
SuMixer::SuMixer(std::string label)
	: siospc::combine("SuMixer", label)
	, m_gain_master(0.5f)
	, m_gain_chan_a(1.0f)
	, m_peak_chan_a(0.0f)
	, m_gain_chan_b(1.0f)
	, m_peak_chan_b(0.0f)
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

		auto lpeaka = 0.0f;
		auto lpeakb = 0.0f;

		for(auto i = 0u; i < total; i++) { 
			auto sampleA = m_chan_a[i];
			auto sampleB = m_chan_b[i];
			
			if((sampleA *= m_gain_chan_a) > lpeaka) lpeaka = sampleA;
			if((sampleB *= m_gain_chan_b) > lpeakb) lpeakb = sampleB;

			m_chan_a[i] = (sampleA + sampleB) * m_gain_master;
		}
		m_chan_b.free();
		feed_out(m_chan_a, AudioOut);
}

void SuMixer::single_channel() {

	if(input_active(ChannelA) && m_chan_a) {

		auto total = m_chan_a.block_size();
		auto lpeak = 0.0f;

		for(auto i = 0u; i < total; i++) {
			auto sample = m_chan_a[i];
			if((sample *= m_gain_chan_a) > lpeak) lpeak = sample;

			m_chan_a[i] = sample * m_gain_master;
		}

		m_peak_chan_b = lpeak;

		feed_out(m_chan_a, AudioOut);

	} else if(input_active(ChannelB) && m_chan_b) {

		auto total = m_chan_b.block_size();
		auto lpeak = 0.0f;

		for(auto i = 0u; i < total; i++) {
			auto sample = m_chan_b[i];
			if((sample *= m_gain_chan_b) > lpeak) lpeak = sample;

			m_chan_b[i] = sample * m_gain_master;
		}

		m_peak_chan_b = lpeak;
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
	event_onchange(gain_type::master, value);
}

void SuMixer::action_gain_chan_a(int value) {
	auto fv = siortn::midi::normalise_velocity128(value);
	m_gain_chan_a = fv;
	m_gain_final_a = m_gain_chan_a * m_gain_master;
	event_onchange(gain_type::channel_a, value);
}

void SuMixer::action_gain_chan_b(int value) {
	auto fv = siortn::midi::normalise_velocity128(value);
	m_gain_chan_b = fv;
	m_gain_final_b = m_gain_chan_b * m_gain_master;
	event_onchange(gain_type::channel_b, value);
}

void SuMixer::listen_onchange(std::weak_ptr<std::function<void(SuMixer::gain_type, int)>> cb) {
	m_onchange_listeners.push_back(cb);
}

void SuMixer::event_onchange(SuMixer::gain_type type, int value) {
	for(auto& wptr : m_onchange_listeners) {
		if(auto shr = wptr.lock()) {
			(*shr)(type, value);
		}
	}
}

float SuMixer::probe_channel_peak(int channel) {

	auto p = 0.0f;

	if(channel) {
		p = m_peak_chan_a;
	} else {
		p = m_peak_chan_b;
	}

	return p;
}
UnitBuilder(SuMixer);
