#include <iostream>
#include <sstream>
#include <signal.h>
#include <algorithm>

#include "framework/routine/sound.hpp"
#include "SuAlsa.hpp"
using namespace strangeio;
using namespace strangeio::component;

using channel_area = snd_pcm_channel_area_t;
#if !DEVBUILD
	#error The testing suite requires DEVBUILD to be enabled
#endif

#define HW_MIN_PERIOD 128ul
#define HW_MAX_PERIOD 1024ul

SuAlsa::SuAlsa(std::string label)
	: dispatch("SuAlsa", label)
	, m_cfg_period_size(0)
	, m_in_driver(0)
	, m_alsa_dev(std::string("default"))
	, m_running(false)
	, m_active(false)
	
{
	add_input("audio_in");
}

SuAlsa::~SuAlsa() {
	fclose(m_fp);
}

cycle_state SuAlsa::cycle() {
	if(m_buffer) {
		flush_samples();
	}

	return cycle_state::complete;
}

void SuAlsa::feed_line(memory::cache_ptr samples, int line) {
	m_buffer = samples;
}
#include "framework/routine/debug.hpp"

void SuAlsa::flush_samples() {
	auto profile = global_profile();
	
	int err;
	std::stringstream ss;
	const channel_area *areas;

	// clear the held buffer
	{
		auto local_buffer = m_buffer;
		auto intw = cache_alloc(1);
		siortn::sound::interleave2(*local_buffer, *intw, profile.period);

		if(snd_pcm_state(m_handle) == -EPIPE) {
			std::cerr << "State error : " << std::endl;
			snd_pcm_recover(m_handle, -EPIPE, 1);
		}
		
		snd_pcm_uframes_t offset, frames = profile.period;
		
		auto available = snd_pcm_avail_update(m_handle);

		if((err = snd_pcm_mmap_begin(m_handle, &areas, &offset, &frames)) < 0) {
			std::cerr << "ALSA mmap error" << std::endl;
		}
		local_buffer.copy_to(((PcmSample*)areas[0].addr)+offset);
		if((err = snd_pcm_mmap_commit(m_handle, offset, frames)) < 0) {
			std::cerr << "ALSA mmap commit error\t" << snd_strerror(err) << std::endl;
			if(err == -EPIPE) {
				snd_pcm_recover(m_handle, err, 1);
			}
		} else {
			std::cout << "Wrote " << err << " Frames" << std::endl;
		}
		
		if(available <= profile.period && available >= 0) {
			snd_pcm_start(m_handle);
		}

	}
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


		m_signal = new std::thread([this](){
		
			/*--------------------------------*\
			 * SuAlsa cycle management thread *
			\*--------------------------------*/

			m_active = true;
			std::stringstream ss;
			
			auto num_fd = snd_pcm_poll_descriptors_count(m_handle);
			m_pfd = new pollfd[num_fd];
			num_fd = snd_pcm_poll_descriptors(m_handle, m_pfd, num_fd);
			
			ss << num_fd << " descriptor(s) active";
			log(ss.str());
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
		});
		m_running = true;	
	}
	log("Initialised");
	
	return cycle_state::complete;
}

siocom::cycle_state SuAlsa::resync(strangeio::component::sync_flag flags){
	
	auto profile = unit_profile();
	if(profile.state == (int) line_state::active) {
		m_signal_cv.notify_one();
		auto channels = global_profile().channels;
		m_areas = new channel_area[channels]{(channel_area{0})};

	}

	return cycle_state::complete;
}

void SuAlsa::set_configuration(std::string key, std::string value) {
	if(key == "period_size") {
		m_cfg_period_size = std::stoi(value);
	} else if(key == "device") {
		m_alsa_dev = value;
	}
}

siocom::cycle_state SuAlsa::init_hwparams() {
	snd_pcm_hw_params_t *hw_params;
	int err, dir = 0;
	std::stringstream ss;
	
	ss << "Device: " << m_alsa_dev << "";
	log(ss.str());
	ss.str("");

	if ((err = snd_pcm_open (&m_handle, m_alsa_dev.c_str(), SND_PCM_STREAM_PLAYBACK,0)) < 0) {
	//if ((err = snd_pcm_open (&m_handle, m_alsa_dev.c_str(), SND_PCM_STREAM_PLAYBACK,SND_PCM_NONBLOCK)) < 0) {
		log("# cannot open audio device - ");
		log(std::string(snd_strerror(err)));
		return cycle_state::error;
	}

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


	// ---- Print the HW parameters
	log("Dumping hardware parameters: ");
	snd_output_t* out;
	snd_output_stdio_attach(&out, stdout, 0);
	snd_pcm_dump_hw_setup(m_handle, out);
	
	auto period_size = 0ul;
	if ((err = snd_pcm_hw_params_get_period_size (hw_params, &period_size, &dir)) < 0) {
		log("# cannot get period size - ");
			log(std::string(snd_strerror(err)));
	}

	register_metric(profile_metric::period, (signed int)period_size);
	snd_pcm_uframes_t buffer_size;
	if ((err = snd_pcm_hw_params_get_buffer_size (hw_params, &buffer_size)) < 0) {
		log("cannot get buffer size - ");
		log(std::string(snd_strerror(err)));
	}

	auto sample_rate = 0u;
	if ((err = snd_pcm_hw_params_get_rate (hw_params, &sample_rate, &dir)) < 0) {
		log("cannot get sample rate - ");
		log(std::string(snd_strerror(err)));
	}
	register_metric(profile_metric::fs, (signed int)sample_rate);
	
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
	
	if((err = snd_pcm_sw_params_set_avail_min(m_handle, sw_params, unit_profile().period)) < 0) {
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

void SuAlsa::poll_loop(int num) {
	unsigned short rev;
	snd_pcm_sframes_t lat, av;
	std::stringstream ss;

	while(1) {
		poll(m_pfd, num, -1);

		snd_pcm_poll_descriptors_revents(m_handle, m_pfd, num, &rev);
		if(unit_profile().state == (int)line_state::inactive) {
				// if inactive, don't bother triggering a cycle
				log("Line inactive");
				break;
		}
		if(!m_running) break;
		
		if(rev & POLLOUT) {
			snd_pcm_avail_delay(m_handle, &av, &lat);
			trigger_cycle();

		} else if(rev & POLLERR) {
			log("I/O Error");
			break;
		} else {
			continue;
		}
	}
}

UnitBuilder(SuAlsa);
