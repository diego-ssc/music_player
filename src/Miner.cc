#include <filesystem>
#include <sqlite3.h>
#include "Miner.h"

Miner::Miner(std::string dir_path, std::string database_path) {
  this->dir_path = dir_path;
  this->database_path = database_path;
}

std::string Miner::get_dir_path() {
  return this->dir_path;
}

std::string Miner::get_database_path() {
  return this->database_path;
}



