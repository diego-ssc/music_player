// ninja -C build/ ../test/test_miner.cc && ./build/test_miner
/*
 * Copyright © 2022 Diego S.
 *
 */

#define CATCH_CONFIG_MAIN

#include <catch2/catch_test_macros.hpp>
#include "Miner.h"

TEST_CASE("Miner_Constructor", "[miner]") {
  auto miner = Miner("./music", "./database.db");
  SECTION("getters") {
    REQUIRE((miner.get_dir_path()).compare("./music") == 0);
    REQUIRE((miner.get_database_path()).compare("./database.db") == 0);
  }
}
