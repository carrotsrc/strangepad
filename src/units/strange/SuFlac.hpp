#ifndef SUFLAC_HPP__
#define SUFLAC_HPP__

#include "framework/component/unit.hpp" // Base class: strangeio::component::unit

class SuFlac : public strangeio::component::unit
{
public:
	SuFlac(std::string label);
	~SuFlac();

public:
	strangeio::component::cycle_state cycle();
	void feed_line(strangeio::memory::cache_ptr samples, int line);
	strangeio::component::cycle_state init();

#if DEVBUILD

	void db_load_file(std::string path) { load_file(path); };
	PcmSample* db_buffer() { return m_buffer; };
	unsigned int db_buf_size() { return m_buf_size; };
	void db_reset_buffer(unsigned int total_samples) { reset_buffer(total_samples); };

	strangeio::memory::cache_ptr db_cache() { return m_cptr; };
	void db_cache_chunk() { cache_chunk(); };
#endif

protected:
	strangeio::component::cycle_state resync();
private:

	strangeio::memory::cache_ptr m_cptr;

	PcmSample* m_buffer;
	PcmSample* m_position;

	unsigned int m_buf_size, m_count, m_samples_played;
	unsigned int m_period_size;

	void load_file(std::string path);
	void cache_chunk();
	void reset_buffer(unsigned int num_samples);
};

#endif // SUFLAC_HPP__
