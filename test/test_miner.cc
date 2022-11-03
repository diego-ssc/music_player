// ninja -C build/ ../test/test_miner.cc && ./build/test_miner
/*
 * Copyright © 2022 Diego S.
 *
 */

#define CATCH_CONFIG_MAIN

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include "Miner.h"


TEST_CASE("Miner_Constructor", "[miner]") {
  auto miner = Miner("./music");
  SECTION("getter") {
    REQUIRE(((miner.get_dir_path()).string()).compare("./music") == 0);
  }
}

int main(int argc, char*argv []) {
  int result = Catch::Session().run(argc, argv);
  
  return result;
}
