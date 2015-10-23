#include "catch.hpp"
#include "framework/alias.hpp"
#include "framework/config/assembler.hpp"
#include "framework/routine/system.hpp"


TEST_CASE("Assemble SuFlac and SuAlsa", "[rig::flacalsa]") {
	siocfg::assembler as;
	auto sys = siortn::system::setup(as, "SuFlacSuAlsa.cfg", 32);

	REQUIRE(sys->has_unit("flac") == true);
	REQUIRE(sys->has_unit("alsa") == true);

	auto flac = sys->get_unit("flac").lock();
	auto alsa = sys->get_unit("alsa").lock();

	sys->warmup();
	auto profile = sys->global_profile();
	REQUIRE(profile.channels == 2);

	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	REQUIRE(flac->get_configuration("num_cached") == "5");
	sys->start();
	flac->set_configuration("kick_start", "");
	auto pr = 0u;
	std::cin >> pr;
}