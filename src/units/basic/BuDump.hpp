#ifndef BUDUMP_HPP
#define	BUDUMP_HPP
#include "framework/alias.hpp"
#include "framework/spec/dispatch.hpp"
#include <fstream>

class BuDump : public strangeio::spec::dispatch {
public:
    enum class state{ ready };
    BuDump(std::string label);
    virtual ~BuDump();

    void feed_line(siomem::cache_ptr samples, int line);
    void set_configuration(std::string key, std::string value);

private:
    siomem::cache_ptr m_cptr;
    
    std::string m_output, m_filename;
    std::ofstream m_of;
    
    unsigned int m_delay_us, m_frames, m_samples;
    

    siocom::cycle_state cycle();
    siocom::cycle_state init();
    siocom::cycle_state resync(siocom::sync_flag flags);
    
    bool exists(std::string out);
};

#endif	/* BUDUMP_HPP */
