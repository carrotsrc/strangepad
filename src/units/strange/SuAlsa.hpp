#ifndef SUALSA_HPP
#define SUALSA_HPP

#include <thread>
#include <atomic>
#include <condition_variable>

#include <alsa/asoundlib.h>

#include "framework/component/unit.hpp" // Base class: strangeio::component::unit

class SuAlsa : public strangeio::component::unit
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

private:
	// Buffer
	strangeio::memory::cache_ptr m_buffer;

	// Alsa variables
	snd_pcm_t *m_handle;
	snd_async_handler_t *m_cb;
	snd_pcm_uframes_t m_trigger_level, m_fperiod;
	unsigned int m_max_periods;

	// safe signal handling
	std::condition_variable m_signal_cv;
	std::mutex m_signal_mutex;
	std::thread* m_signal;
	bool m_running, m_active;

	void flush_samples();
};

#endif // SUALSA_HPP