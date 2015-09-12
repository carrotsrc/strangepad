#ifndef SUDELAY_HPP__
#define SUDELAY_HPP__

#include <memory>
#include <vector>

#include "framework/alias.hpp"
#include "framework/component/unit.hpp"


class SuDelay : public siocom::unit {
public:
	SuDelay(std::string label);
	~SuDelay();

public:
	enum working_state {
		passing, loading, ready, streaming
	};

	void feed_line(siomem::cache_ptr samples, int line);

	void listen_onchange(std::weak_ptr<std::function<void(SuDelay::working_state)> > cb);


protected:
	siocom::cycle_state cycle();
	siocom::cycle_state init();

private:
	std::vector<std::weak_ptr<std::function<void(SuDelay::working_state) > > > m_onchange_listeners;
	void event_onchange(SuDelay::working_state state);
};

using sudelay_onchange_cb = std::function<void(SuDelay::working_state)>;
using sudelay_onchange_wptr = std::weak_ptr<sudelay_onchange_cb>;
using sudelay_onchange_sptr = std::shared_ptr<sudelay_onchange_cb>;

#endif // SUDELAY_HPP
