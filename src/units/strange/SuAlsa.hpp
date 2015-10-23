#ifndef SUALSA_HPP
#define SUALSA_HPP

#include <thread>
#include <atomic>
#include <condition_variable>

#include <alsa/asoundlib.h>

#include <stdio.h>

#include "framework/alias.hpp"
#include "framework/component/unit.hpp" // Base class: strangeio::component::unit
#include "framework/spec/dispatch.hpp" // Base class: strangeio::component::unit

class SuAlsa : public strangeio::spec::dispatch
{
public:
	enum work_state {
		idle, ///< Unitialised
		inititalise, ///< Initialising the unit
		ready, ///< Ready to receive data
		priming, ///< Priming the delay buffer
		streaming, ///< Loading delay buffer
		flushing, ///< Flushing the delay buffer
		paused, ///< Received a pause state
		waiting
	};

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
	FILE *m_fp;

	// Alsa variables
	snd_pcm_t *m_handle;
	snd_async_handler_t *m_cb;
	snd_pcm_uframes_t m_trigger_level, m_fperiod;
	unsigned int m_max_periods;
	unsigned int m_cfg_period_size;
	std::atomic<int> m_in_driver, m_is_active;

	struct pollfd *m_pfd;
        std::string m_alsa_dev;

	// safe signal handling
	std::condition_variable m_signal_cv;
	std::mutex m_signal_mutex;
	std::thread* m_signal;
	bool m_running, m_active;

	void flush_samples();
        void init_swparams();
        void poll_loop(int num);
};

#endif // SUALSA_HPP
