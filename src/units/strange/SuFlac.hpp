#ifndef SUFLAC_HPP__
#define SUFLAC_HPP__
#include <atomic>
#include <memory>
#include <array>
#include <vector>

#include "framework/alias.hpp"
#include "framework/component/unit.hpp" // Base class: strangeio::component::unit

class SuFlac : public siocom::unit
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

	const PcmSample* probe_flac_data() const;
	std::string probe_flac_path() const;
	unsigned int probe_total_spc() const;

protected:
	siocom::cycle_state resync();

private:
	std::array< siomem::cache_ptr, 5 > m_cptr;
	std::atomic_int m_num_cached, m_rindex, m_windex;

	PcmSample* m_buffer;
	PcmSample* m_position;

	unsigned int m_buf_size, m_remain, m_samples_played;
	unsigned int m_period_size, m_num_channels;

	std::string m_flac_path;

	std::vector<std::weak_ptr<std::function<void(SuFlac::working_state) > > > m_onchange_listeners;

	void load_file();
	void cache_chunk();
	void reset_buffer(unsigned int num_samples);

	void event_onchange(SuFlac::working_state state);

#if DEVBUILD
public:
	void db_load_file(std::string path) { m_flac_path = path; load_file(); };
	PcmSample* db_buffer() { return m_buffer; };
	unsigned int db_buf_size() { return m_buf_size; };
	void db_reset_buffer(unsigned int total_samples) { reset_buffer(total_samples); };

	strangeio::memory::cache_ptr db_cache() { return m_cptr[m_rindex++]; };
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
