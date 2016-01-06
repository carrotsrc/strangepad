#ifndef SUFLAC_HPP__
#define SUFLAC_HPP__
#include <atomic>
#include <mutex>
#include <memory>
#include <array>
#include <vector>

#include "framework/alias.hpp"
#include "framework/spec/mainline.hpp" // Base class: strangeio::component::unit

#define SuFlacCacheSize 2

class SuFlac : public siospc::mainline
{
public:
	SuFlac(std::string label);
	~SuFlac();

public:
	enum working_state {
		idle,
		loading,
		prestream,
		streaming,
		paused,
		zero,
		bpm_update,

		sync_streaming,
		sync_paused,
	};

public:
	siocom::cycle_state cycle();
	void feed_line(siomem::cache_ptr samples, int line);
	siocom::cycle_state init();
	
	void set_configuration(std::string key, std::string value);

	// listener registration
	void listen_onchange(std::weak_ptr<std::function<void(SuFlac::working_state)>> cb);

	// Action methods for the pad
	void action_load_file(std::string path);
	void action_start_stream();
	void action_jump_to_sample(int sample);

	const PcmSample* probe_flac_data() const;
	std::string probe_flac_path() const;
	unsigned int probe_total_spc() const;
	int probe_progress() const;
	int probe_bpm() const;
	
	void set_bpm(int bpm);

protected:
	siocom::cycle_state resync(siocom::sync_flag flags);

private:
	std::array< siomem::cache_ptr, SuFlacCacheSize > m_cptr;
        std::array< PcmSample*, SuFlacCacheSize > m_position_history; 
        int m_ph_start, m_ph_end;
        
	std::mutex m_buffer_mutex;
	std::atomic_int m_num_cached, m_rindex, m_windex;

	PcmSample* m_buffer;
	PcmSample* m_position, *m_jump_pos;
        

	unsigned int m_buf_size, m_remain;
	unsigned int m_period_size, m_old_period, m_num_channels;
	int m_track_bpm;
	signed int m_samples_played, m_samples_cached;
	std::atomic<bool> m_jump, m_final;
	working_state m_ws;

	std::string m_flac_path;
	bool m_bpm_sync, m_downstream_fill;

	std::vector<std::weak_ptr<std::function<void(SuFlac::working_state) > > > m_onchange_listeners;

	void load_file();
	void run_prefill();
	void cache_chunk();
	void reset_buffer(unsigned int num_samples);
	void clear_cache();
        void reset_cache();

	void event_onchange(SuFlac::working_state state);

#if DEVBUILD
public:
	void db_load_file(std::string path) { m_flac_path = path; load_file(); };
	PcmSample* db_buffer() { return m_buffer; };
	unsigned int db_buf_size() { return m_buf_size; };
	void db_reset_buffer(unsigned int total_samples) { reset_buffer(total_samples); };

	strangeio::memory::cache_ptr db_cache() { return std::move(m_cptr[m_rindex++]); };
	int db_cache_size() { return m_num_cached; };
	void db_cache_chunk() { cache_chunk(); };
	
	void db_kick_start() { trigger_cycle(); };
	
	std::string get_configuration(std::string key) {
		if(key == "num_cached") {
			return std::to_string(m_num_cached);
		}
		return std::string();
	}
#endif

};

using suflac_onchange_cb = std::function<void(SuFlac::working_state)>;
using suflac_onchange_wptr = std::weak_ptr<suflac_onchange_cb>;
using suflac_onchange_sptr = std::shared_ptr<suflac_onchange_cb>;

#endif // SUFLAC_HPP__
