#ifndef BUPING_HPP
#define	BUPING_HPP
#include <atomic>

#include "framework/alias.hpp"
#include "framework/component/unit.hpp"
#include "framework/routine/debug.hpp"

class BuPing : public siocom::unit {
public:
	enum working_state { passing, pinging };
	BuPing(std::string label);
	virtual ~BuPing();

	void feed_line(siomem::cache_ptr samples, int line);

private:	
	void write_ping();
	siocom::cycle_state cycle();
	siocom::cycle_state init();
	siocom::cycle_state resync(siocom::sync_flag flags);
	
	strangeio::memory::cache_ptr m_ptr;
	siortn::debug::tp m_current, m_last;
	int m_diff, m_ping_num, m_beat;
	unsigned int m_bpm, m_bpm_centre, m_modifier_on;
	unsigned int m_fc, m_x, m_cycle, m_prog, m_ping_len, m_ping_smp;
	
	float m_2pi;
	bool m_in_ping;
	std::atomic<working_state> m_state;	
};

#endif	/* BUPING_HPP */
