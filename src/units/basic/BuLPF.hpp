#ifndef BULPF_HPP
#define	BULPF_HPP

#include "framework/alias.hpp"
#include "framework/component/unit.hpp"

class BuLPF : public siocom::unit {
public:
	BuLPF(std::string label);
	virtual ~BuLPF();

	void feed_line(siomem::cache_ptr samples, int line);

private:
	siomem::cache_ptr m_cache;
	float m_zb1c1, m_zb1c2;
	float m_a0, m_b1;
	
	siocom::cycle_state cycle();
	siocom::cycle_state init();
	siocom::cycle_state resync(siocom::sync_flag flags);
};

#endif	/* BULPF_HPP */

