#ifndef SUFLAC_HPP__
#define SUFLAC_HPP__

#include "framework/component/unit.hpp" // Base class: strangeio::component::unit

class SuFlac : public strangeio::component::unit
{
public:
	SuFlac(std::string label);
	~SuFlac();

public:
	virtual
strangeio::component::cycle_state cycle();
	virtual
void feed_line(strangeio::memory::cache_ptr samples, int line);
	virtual
strangeio::component::cycle_state init();
};

#endif // SUFLAC_HPP__
