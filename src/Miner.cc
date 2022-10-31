/*
 * Copyright © 2022 Diego S.
 *
 */

#include "Miner.h"
#include <sqlite3.h>
#include <filesystem>
#include <string>

Miner::Miner(std::string dir_path) {
  this->dir_path = dir_path;
}

std::filesystem::path Miner::get_dir_path() {
  return this->dir_path;
}

int Miner::add_to_database() {
  if (!database_exists())
    return -1;

  return 0;
}

bool Miner::database_exists() {
  std::filesystem::path p =
    "~/.local/share/music_player/database.db";

  return std::filesystem::exists(p);
}
