#ifndef SUMIXER_HPP__
#define SUMIXER_HPP__

#include <memory>

#include "framework/alias.hpp"
#include "framework/spec/combine.hpp" // Base class: strangeio::component::unit

class SuMixer : public siospc::combine {
public:
	SuMixer(std::string label);
	~SuMixer();

public:
	enum gain_type {
		master, channel_a, channel_b
	};

	void feed_line(siomem::cache_ptr samples, int line);

	void action_gain_master(int value);
	void action_gain_chan_a(int value);
	void action_gain_chan_b(int value);

	void listen_onchange(std::weak_ptr<std::function<void(SuMixer::gain_type, int)>> cb);

	float probe_channel_peak(int channel);
protected:
	siocom::cycle_state cycle();
	siocom::cycle_state init();

private:
	siomem::cache_ptr m_chan_a, 
					  m_chan_b;

	std::vector<std::weak_ptr<std::function<void(SuMixer::gain_type, int) > > > m_onchange_listeners;
	float m_gain_master,
		  m_gain_chan_a, m_gain_final_a, m_peak_chan_a,
		  m_gain_chan_b, m_gain_final_b, m_peak_chan_b;

	void mix_channels();
	void single_channel();
	
	void event_onchange(gain_type type, int value);
};

using sumixer_onchange_cb = std::function<void(SuMixer::gain_type, int)>;
using sumixer_onchange_wptr = std::weak_ptr<sumixer_onchange_cb>;
using sumixer_onchange_sptr = std::shared_ptr<sumixer_onchange_cb>;

#endif // SUMIXER_HPP
