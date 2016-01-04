#include <iostream>
#include <sstream>
#include <signal.h>
#include <algorithm>
#include <iomanip>
#include <sched.h>

#include "framework/routine/sound.hpp"
#include "framework/thread/scheduled.hpp"
#include "SuAlsa.hpp"


#define SIO_SCHED SCHED_FIFO

using namespace strangeio;
using namespace strangeio::component;
using mem_order = std::memory_order;
using channel_area = snd_pcm_channel_area_t;

#define PATH_NONE 0
#define PATH_WAKEUP 1

#define DEBUG_PATH PATH_NONE

#define HW_MIN_PERIOD 128ul
#define HW_MAX_PERIOD 1024ul

#if ALSA_IRQ == AIRQ_ASYNC
static void pcm_trigger_callback(snd_async_handler_t *);
#endif

SuAlsa::SuAlsa(std::string label)
	: dispatch("SuAlsa", label)
	, m_alsa_dev(std::string("default"))

	, m_cfg_period_size(0)
	, m_trigger(0)
	, m_in_mmap(0)
	, m_running(false)
	, m_active(false)

	, m_delay_trigger(0)
	, m_delay_flush(0)
{
	m_state.store(state::uninit);
	m_tps = m_tpe = siortn::debug::zero_timepoint();
	m_cycling.clear();
	add_input("audio_in");
	
	m_schpolicy.policy = SCHED_OTHER;
	m_schpolicy.priority = 0;
	m_schpolicy.cpu_affinity = -1;
}

SuAlsa::~SuAlsa() {
}

cycle_state SuAlsa::cycle() {
	if(m_buffer) {
		flush_samples();
	}

	return cycle_state::complete;
}

void SuAlsa::feed_line(memory::cache_ptr samples, int line) {
	
	if(!m_buffer) m_buffer = samples;
		
}

#if ALSA_IRQ == AIRQ_POLL
#include "framework/routine/debug.hpp"
void SuAlsa::poll_loop(int num) {

	unsigned short rev;
	snd_pcm_sframes_t delay;
	
	while(1) {

		poll(m_pfd, num, -1);

		snd_pcm_poll_descriptors_revents(m_handle, m_pfd, num, &rev);
		if(unit_profile().state == (int)line_state::inactive) {
				// if inactive, don't bother triggering a cycle
				break;
		}
		
		if(!m_running) break; 
		
		if(rev & POLLOUT) {



			if(m_state.load() == state::streaming) {
				snd_pcm_avail_update(m_handle);
				snd_pcm_delay(m_handle, &delay);

				m_delay_trigger = delay;

				if(delay <= m_trigger) {

					if(m_cycling.test_and_set() == true) {
						// STICKING POINT
						continue;
					}
//					log("Triggering cycle");
					trigger_cycle();
					if(delay <= m_trigger/2) trigger_cycle();
					
					sched_yield();
				}
			}
		} else if(rev & POLLERR) {
			log("I/O Error");
			break;
		} else {
			continue;
		}
	}
}
#endif

void SuAlsa::flush_samples() {
	m_tpe = siortn::debug::clock_time();
	auto profile = unit_profile();

	int err;
	std::stringstream ss;
	const channel_area *areas;
	snd_pcm_sframes_t delay = 0;
	snd_pcm_uframes_t offset, frames = profile.period;
	// clear the held buffer
	{
		auto local_buffer = m_buffer;
		auto intw = cache_alloc(1);
		siortn::sound::interleave2(*local_buffer, *intw, profile.period);
		
		snd_pcm_avail_update(m_handle);
		
		if((err = snd_pcm_mmap_begin(m_handle, &areas, &offset, &frames)) < 0) {
			std::cerr << "ALSA mmap error" << std::endl;
		}

		snd_pcm_delay(m_handle, &delay);
		m_delay_flush = delay;

		// File write
		intw.copy_to(((PcmSample*)areas[0].addr) + (offset*2), profile.period*2);
		
	}

	if((err = snd_pcm_mmap_commit(m_handle, offset, frames)) < 0) {
		
		std::cerr << "ALSA mmap commit error\t" 
			<< snd_strerror(err) << std::endl;

		if(err == -EPIPE) {
			snd_pcm_recover(m_handle, err, 0);
			snd_pcm_avail_update(m_handle);
			m_state.store(state::priming);
			trigger_cycle();
		}
	} else {

		snd_pcm_avail_update(m_handle);
		m_in_mmap += err;

		if(m_state.load() == state::priming) {

			if(m_in_mmap >= m_trigger) {
				/*
				if(m_cycling.test_and_set()) {
					return;
				}*/

				if( (err = snd_pcm_start(m_handle)) < 0) {
					std::cerr << "Stream start error\t" 
					<< snd_strerror(err) << std::endl;
				}

				m_state.store(state::streaming);

				snd_pcm_delay(m_handle, &delay);
				trigger_cycle();
				
				return;

			} else {
				snd_pcm_delay(m_handle, &delay);
				trigger_cycle();
			}

		}

	}
	
	m_cycling.clear();
}

cycle_state SuAlsa::init() {

	auto err = 0;
	if(init_hwparams() == cycle_state::error) return cycle_state::error;
	init_swparams();

	if ((err = snd_pcm_prepare (m_handle)) < 0) {
		log("# cannot prepare audio interface - ");
		log(std::string(snd_strerror(err)));
		return cycle_state::error;
	}

	if(!m_running) {
		

#if ALSA_IRQ == AIRQ_POLL
		m_signal =	new siothr::scheduled(m_schpolicy, [this](){

			/*--------------------------------*\
			 * SuAlsa cycle management thread *
			\*--------------------------------*/

			m_active = true;
			std::stringstream ss;
			
			auto num_fd = snd_pcm_poll_descriptors_count(m_handle);
			m_pfd = new pollfd[num_fd];
			num_fd = snd_pcm_poll_descriptors(m_handle, m_pfd, num_fd);
			
			ss << num_fd << " descriptor(s) active";
			//log(ss.str());
			ss.str("");
			std::unique_lock<std::mutex> ul(m_signal_mutex);

			while(1) {
				if(!m_running) {
					break;
				}
				
				this->m_signal_cv.wait(ul);
				poll_loop(num_fd);
			}

			m_active = false;
			
			/* ---- End of thread ---- */
		}, ulabel());
#elif ALSA_IRQ == AIRQ_ASYNC
		m_signal =	new siothr::scheduled(m_schpolicy, [this](){

			/*--------------------------------*\
			 * SuAlsa cycle management thread *
			 * Async							  *
			\*--------------------------------*/

			std::unique_lock<std::mutex> ul(m_signal_mutex);
			snd_pcm_sframes_t delay;

			while(1) {
				m_signal_cv.wait(ul);

				if(!m_running) {
					ul.unlock();
					break;
				}

				if(unit_profile().state == (int)line_state::inactive) {
					// if inactive, don't bother triggering a cycle
					continue;
				}
				

				snd_pcm_avail_update(m_handle);
				snd_pcm_delay(m_handle, &delay);


				if(delay <= m_trigger) {

					if(m_cycling.test_and_set() == true) {
						// STICKING POINT
						continue;
					}
//					log("Triggering cycle");
					trigger_cycle();
					if(delay <= m_trigger/2)
						trigger_cycle();
					
					sched_yield();
				}
			}
			m_active = false;			
			/* ---- End of thread ---- */
		}, ulabel());

		//	Handle async signals safely
		auto *func = new std::function<void(void)>([this](){
			m_signal_cv.notify_one();			
		});

		snd_async_add_pcm_handler(&m_cb, m_handle, &pcm_trigger_callback, (void*)func);
#endif
		
		m_running = true;	
	}
	m_state.store(state::ready);
	log("Initialised");
	
	return cycle_state::complete;
}

siocom::cycle_state SuAlsa::resync(strangeio::component::sync_flag){
	auto profile = unit_profile();
	if(profile.state == (int) line_state::active) {
		
		if(m_state.load() == state::ready) {
			m_state.store(state::priming);
		}
		
		m_signal_cv.notify_one();
	}

	return cycle_state::complete;
}

void SuAlsa::set_configuration(std::string key, std::string value) {
	if(key == "period_size") {
		m_cfg_period_size = std::stoi(value);
	} else if(key == "device") {
		m_alsa_dev = value;
	} else if(key == "trigger") {
		m_trigger = std::stoi(value);
	} else if(key == "sched_priority") {
		m_schpolicy.priority = std::stoi(value);
	} else if(key == "cpu_affinity") {
		m_schpolicy.cpu_affinity = std::stoi(value);	
	} else if(key == "sched_policy") {
		
		if(value == "SCHED_FIFO") {
			m_schpolicy.policy = SCHED_FIFO;
		} else if(value == "SCHED_RR") {
			m_schpolicy.policy = SCHED_RR;
		} else if(value == "SCHED_OTHER") {
			m_schpolicy.policy = SCHED_OTHER;
		} else if(value == "SCHED_BATCH") {
			#ifdef __USE_GNU
			m_schpolicy.policy = SCHED_BATCH;
			#endif
		} else if(value == "SCHED_IDLE") {
			#ifdef __USE_GNU
			m_schpolicy.policy = SCHED_IDLE;
			#endif
		}	
	}
}

siocom::cycle_state SuAlsa::init_hwparams() {
	snd_pcm_hw_params_t *hw_params;
	int err, dir = 0;
	std::stringstream ss;
	
	ss << "Device: " << m_alsa_dev << "";
	log(ss.str());
	ss.str("");

#if ALSA_IRQ == AIRQ_ASYNC
	if ((err = snd_pcm_open (&m_handle, m_alsa_dev.c_str(), SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK)) < 0) {
		log("# cannot open audio device - ");
		log(std::string(snd_strerror(err)));
		return cycle_state::error;
	}

	
#elif ALSA_IRQ == AIRQ_POLL
	
	if ((err = snd_pcm_open (&m_handle, m_alsa_dev.c_str(), SND_PCM_STREAM_PLAYBACK,0)) < 0) {
		log("# cannot open audio device - ");
		log(std::string(snd_strerror(err)));
		return cycle_state::error;
	}	
#endif

	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		log("# cannot allocated hardware param struct - ");
		log(std::string(snd_strerror(err)));
		return cycle_state::error;
	}

	if ((err = snd_pcm_hw_params_any (m_handle, hw_params)) < 0) {
		log("# cannot init hardware param struct - ");
		log(std::string(snd_strerror(err)));
		return cycle_state::error;
	}

	if ((err = snd_pcm_hw_params_set_access (m_handle, hw_params, SND_PCM_ACCESS_MMAP_INTERLEAVED)) < 0) {
		log("# cannot set access type - ");
		log(std::string(snd_strerror(err)));
		return cycle_state::error;
	}

	if ((err = snd_pcm_hw_params_set_format (m_handle, hw_params, SND_PCM_FORMAT_FLOAT_LE)) < 0) {
		log("# cannot set format - ");
		log(std::string(snd_strerror(err)));
		return cycle_state::error;
	}

	// Sample rate
	auto fs = 44100u;
	if ((err = snd_pcm_hw_params_set_rate_near (m_handle, hw_params, &fs, &dir)) < 0) {
		log("# cannot set sample rate - ");
		log(std::string(snd_strerror(err)));
		return cycle_state::error;
	}

	// Channels
	if ((err = snd_pcm_hw_params_set_channels (m_handle, hw_params, 2)) < 0) {
		log("# cannot set channels - ");
		log(std::string(snd_strerror(err)));
		return cycle_state::error;
	}

	auto	min_period_size = HW_MIN_PERIOD, 
			max_period_size = HW_MAX_PERIOD;

	if(( err = 
		snd_pcm_hw_params_set_period_size_minmax(
			m_handle, hw_params,
			&min_period_size, &dir,
			&max_period_size, &dir
		)) < 0 ) {
		log("# cannot set minmax period size - ");
		log(std::string(snd_strerror(err)));
		return cycle_state::error;
	}

	ss << "min/max: " << min_period_size << "/" << max_period_size;
	
	log(ss.str());
	ss.str("");

	auto actual_period_size = min_period_size;

	if(m_cfg_period_size  > 0
	&& m_cfg_period_size >= min_period_size
	&& m_cfg_period_size <= max_period_size) {
		actual_period_size = m_cfg_period_size;
	}

	if ((err = snd_pcm_hw_params_set_period_size(m_handle, hw_params, actual_period_size, dir)) < 0) {
		ss << "# Cannot set period size to " << actual_period_size << " frames";
		log(ss.str());
		log("~ Setting to configurable minimum");
		actual_period_size = min_period_size;
		if ((err = snd_pcm_hw_params_set_period_size(m_handle, hw_params, min_period_size, dir)) < 0) {
			log("# Failed and bailed!");
			return cycle_state::error;
		}
	}
	
	if ((err = snd_pcm_hw_params_set_periods(m_handle, hw_params, 3, dir)) < 0) {
		log("# cannot set periods - ");
		log(std::string(snd_strerror(err)));
		return cycle_state::error;
	}

	if ((err = snd_pcm_hw_params (m_handle, hw_params)) < 0) {
		log("# cannot set parameters - ");
		log(std::string(snd_strerror(err)));
		return cycle_state::error;
	}

	snd_pcm_hw_params_free (hw_params);
	log("Hardware parameters set");

	auto period_size = 0ul;
	if ((err = snd_pcm_hw_params_get_period_size (hw_params, &period_size, &dir)) < 0) {
		log("# cannot get period size - ");
			log(std::string(snd_strerror(err)));
	}
	register_metric(profile_metric::period, (signed int)period_size);
	if(!m_trigger) m_trigger = period_size * 2;
	
	auto sample_rate = 0u;
	if ((err = snd_pcm_hw_params_get_rate (hw_params, &sample_rate, &dir)) < 0) {
		log("cannot get sample rate - ");
		log(std::string(snd_strerror(err)));
	}
	register_metric(profile_metric::fs, (signed int)sample_rate);

	// ---- Print the HW parameters
	log("Dumping hardware parameters: ");
	snd_output_t* out;
	snd_output_stdio_attach(&out, stdout, 0);
	snd_pcm_dump_hw_setup(m_handle, out);
	
	
	
	return cycle_state::complete;
}

void SuAlsa::init_swparams() {
	std::stringstream ss;
	snd_pcm_sw_params_t *sw_params;
	int err;
	if((err = snd_pcm_sw_params_malloc(&sw_params)) < 0) {
		log("# Unable to allocate software parameters");
		return;
	}
	
	
	if((err =snd_pcm_sw_params_current(m_handle, sw_params)) < 0) {
		log("# Unable to get current software parameters");
		return;
	}

	if((err = snd_pcm_sw_params_set_period_event(m_handle, sw_params,1)) < 0) {
		log("# Unable to set period event");
		return;
	}
	
	if((err = snd_pcm_sw_params_set_avail_min(m_handle, sw_params, unit_profile().period*3)) < 0) {
		log("# Unable to set period minimum available");
		return;
	}

	if((err = snd_pcm_sw_params_set_start_threshold(m_handle, sw_params, 0)) < 0) {
		log("# Unable to set start threshold");
		return;
	}

	if((err = snd_pcm_sw_params(m_handle, sw_params)) < 0) {
		log("# Failed to write software parameters");
		return;
	}
	log("Software parameters set");
	
	// ---- Print the SW parameters
	log("Dumping software parameters: ");
	snd_output_t* out;
	snd_output_stdio_attach(&out, stdout, 0);
	snd_pcm_dump_sw_setup(m_handle, out);
}

/* This is a signal handler. ALSA uses SIGIO to tell the unit that the
 * output buffer is running low and needs to be refilled. This function
 * is executed when the signal is received. The signal is async and is
 * sent to the main process thread. It is important that there are no heap
 * allocations that occur in the callstack from this function because
 * if a heap allocation in the main process is interrupted by the
 * signal, and another heap allocation occurs somewhere from the signal
 * handler there will be a deadlock on __lll_lock_wait_private.
 *
 * No:
 * [m/c/re]alloc()
 * free()
 * new
 * delete
 */

#if ALSA_IRQ == AIRQ_ASYNC
static void pcm_trigger_callback(snd_async_handler_t *cb) {
	auto callback = (std::function<void(void)>*)snd_async_handler_get_callback_private(cb);
	(*callback)();
}
#endif


UnitBuilder(SuAlsa);
