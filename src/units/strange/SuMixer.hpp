#ifndef SUMIXER_HPP__
#define SUMIXER_HPP__

#include "framework/alias.hpp"
#include "framework/spec/combine.hpp" // Base class: strangeio::component::unit

class SuMixer : public siospc::combine {
public:
	SuMixer(std::string label);
	~SuMixer();
	void feed_line(siomem::cache_ptr samples, int line);

	void action_gain_master(int value);
	void action_gain_chan_a(int value);
	void action_gain_chan_b(int value);

protected:
	siocom::cycle_state cycle();
	siocom::cycle_state init();

private:
	siomem::cache_ptr m_chan_a, 
					  m_chan_b;

	float m_gain_master,
		  m_gain_chan_a, m_gain_final_a,
		  m_gain_chan_b, m_gain_final_b;

	void mix_channels();
	void single_channel();
};

#endif // SUMIXER_HPP
