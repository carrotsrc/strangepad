#include "catch.hpp"
#include "suite.hpp"
#include "framework/memory/cache_manager.hpp"
#include "../units/strange/SuFlac.hpp"
TEST_CASE("Creation and deletion", "[SuFlac]") {

	SuFlac unit("test_flac");
	REQUIRE(unit.umodel() == "SuFlac");
	REQUIRE(unit.ulabel() == "test_flac");
	REQUIRE(unit.utype() == strangeio::component::unit_type::mainline);
	REQUIRE(unit.controllable() == true);
}

TEST_CASE("Buffer reset", "SuFlac") {
	SuFlac unit("test_flac");
	unit.db_reset_buffer(1024);
	auto initial_ptr = unit.db_buffer();

	SECTION("Verify initial reset conditions") {
		REQUIRE(unit.db_buffer() != nullptr);
		REQUIRE(unit.db_buf_size() == 1024);
	}

	SECTION("Reset with lower sample count") {
		unit.db_reset_buffer(768);
		REQUIRE(unit.db_buffer() != nullptr);
		REQUIRE(unit.db_buffer() == initial_ptr);
		REQUIRE(unit.db_buf_size() == 768);
	}

	SECTION("Reset with higher sample count") {
		unit.db_reset_buffer(2048);
		REQUIRE(unit.db_buffer() != nullptr);
		REQUIRE(unit.db_buffer() != initial_ptr);
		REQUIRE(unit.db_buf_size() == 2048);
	}
}

TEST_CASE("Load file", "SuFlac") {
	SuFlac unit("test_flac");
	unit.db_load_file("120hz.flac");
	REQUIRE(unit.db_buffer() != nullptr);
	REQUIRE(unit.db_buf_size() > 0);

	auto profile = unit.unit_profile();
	REQUIRE(profile.channels == 2);
}

TEST_CASE("Cache chunk", "SuFlac") {
	strangeio::memory::cache_manager cache(32);
	cache.build_cache(512);
	strangeio::component::sync_profile profile;
	profile.period = 256;
	profile.channels = 2;
	SuFlac unit("test_flac");

	unit.set_cache_utility(&cache);

	auto globsync = (component::sync_flag)component::sync_flags::glob_sync;
	unit.sync_line(profile, globsync, 0);
/*
	SECTION("Fail cache with no buffer") {
		unit.db_cache_chunk();
		auto cptr = unit.db_cache();
		REQUIRE(cptr == false);
	}
*/

	SECTION("Succeed cache with buffer") {
		unit.db_load_file("120hz.flac");
		unit.db_cache_chunk();
		auto cptr = unit.db_cache();
		REQUIRE(cptr == true);
		REQUIRE(unit.db_cache_size() == 5);
	}
}