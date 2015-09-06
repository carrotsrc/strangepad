#include "suite.hpp"
#include "../units/strange/SuFlac.hpp"
TEST_CASE("Creation and deletion", "[SuFlac]") {

	auto unit = new SuFlac("test_flac");
	REQUIRE(unit->umodel() == "SuFlac");
	REQUIRE(unit->ulabel() == "test_flac");
	REQUIRE(unit->utype() == strangeio::component::unit_type::mainline);
	REQUIRE(unit->controllable() == true);
	
}