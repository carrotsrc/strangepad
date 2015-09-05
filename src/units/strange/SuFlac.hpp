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

private:

	PcmSample* m_buffer;
	PcmSample* m_position;

	unsigned int m_buf_size, m_count, m_samples_played;
	void load_file(std::string path);
};

#endif // SUFLAC_HPP__
