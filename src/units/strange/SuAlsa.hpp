#ifndef SUALSA_HPP
#define SUALSA_HPP

#include <thread>
#include <atomic>
#include <condition_variable>

#include <alsa/asoundlib.h>



#include "framework/alias.hpp"
#include "framework/component/unit.hpp" // Base class: strangeio::component::unit
#include "framework/spec/dispatch.hpp" // Base class: strangeio::component::unit
#include "framework/routine/debug.hpp"

#define AIRQ_ASYNC 0
#define AIRQ_POLL 1
#define ALSA_IRQ AIRQ_ASYNC

#define ALSA_DUMP 1

#if ALSA_DUMP
#include <stdio.h>
#endif


class SuAlsa : public strangeio::spec::dispatch
{
public:
	enum class state { uninit, ready, priming, streaming };

	SuAlsa(std::string label);
	~SuAlsa();

public:
	strangeio::component::cycle_state cycle();
	void feed_line(strangeio::memory::cache_ptr samples, int line);
	strangeio::component::cycle_state init();
	void set_configuration(std::string key, std::string value);

protected:
	siocom::cycle_state resync(strangeio::component::sync_flag flags);


private:
	// Buffer
	strangeio::memory::cache_ptr m_buffer;

	// state machine
	std::atomic<state> m_state;
        
	// Alsa variables
	snd_pcm_t *m_handle;
	snd_async_handler_t *m_cb;
	snd_pcm_uframes_t m_trigger_level, m_fperiod;
        
    std::string m_alsa_dev;
	unsigned int m_max_periods, m_cfg_period_size, m_trigger;
        
	std::atomic<unsigned int> m_in_mmap;
    std::atomic_flag m_cycling;
	struct pollfd *m_pfd;

	// poll handler
	std::condition_variable m_signal_cv;
	std::mutex m_signal_mutex;
	siothr::scheduled* m_signal;
	siothr::sched_desc m_schpolicy;
	bool m_running, m_active;

	siortn::debug::tp m_tps, m_tpe;
    
	snd_pcm_sframes_t m_delay_trigger, m_delay_flush;

#if ALSA_DUMP
	FILE* m_fp;
#endif


	void flush_samples();
	void init_swparams();
    siocom::cycle_state init_hwparams();
#if ALSA_IRQ == AIRQ_POLL
	void poll_loop(int num);
#endif
        
};

#endif // SUALSA_HPP
