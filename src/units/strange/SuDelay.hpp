#ifndef SUDELAY_HPP__
#define SUDELAY_HPP__

#include <memory>
#include <vector>
#include <atomic>

#include "framework/alias.hpp"
#include "framework/component/unit.hpp"


class SuDelay : public siocom::unit {
public:
	SuDelay(std::string label);
	~SuDelay();

public:
	enum working_state {
		passing, resetting, priming, ready, filtering
	};

	void feed_line(siomem::cache_ptr samples, int line);

	void listen_onchange(std::weak_ptr<std::function<void(SuDelay::working_state)> > cb);

	void action_start();
	void action_stop();

	void action_mod_decay(float value);
	void action_mod_volume(float value);

protected:
	siocom::cycle_state cycle();
	siocom::cycle_state init();
	siocom::cycle_state resync(siocom::sync_flag flags);

private:
	working_state m_ws;
	unsigned int m_delay_time,  m_delay_size, m_period_size;
	unsigned int m_spms, m_delay_range;
	std::atomic<float> m_a, m_b;

	std::atomic<bool> m_resetting;
	PcmSample* m_delay_buffer, 
				*m_write_r, *m_read_r, *m_end_r, 
				*m_write_l, *m_read_l, *m_end_l,
				*m_start_r, *m_start_l;

	siomem::cache_ptr m_cptr;

	void reset_delay();

	std::vector<std::weak_ptr<std::function<void(SuDelay::working_state) > > > m_onchange_listeners;
	void event_onchange(SuDelay::working_state state);

};

using sudelay_onchange_cb = std::function<void(SuDelay::working_state)>;
using sudelay_onchange_wptr = std::weak_ptr<sudelay_onchange_cb>;
using sudelay_onchange_sptr = std::shared_ptr<sudelay_onchange_cb>;

#endif // SUDELAY_HPP
