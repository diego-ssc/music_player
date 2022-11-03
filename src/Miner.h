/*
 * Copyright © 2022 Diego S.
 *
 */
#pragma once

#include <string>
#include <filesystem>

/**
 * Class to locate the .mp3 files
 * of the user.
 *
 */
class Miner {
  /** The path of the directory where the music
      is located. */
  std::filesystem::path dir_path;
  /** The path of the database. */
  std::filesystem::path database_path;
  /** The name of the current user executing
      the application */
  std::string username;
  
 public:
  /**
   * Class constructor.
   * @param dir_path The path of the target directory.
   *
   */
  Miner(std::string dir_path);

  /**
   * Method that returns the directory path.
   * @return the directory path.
   *
   */
  std::filesystem::path get_dir_path();

  /**
   * Method that adds a song file to the database.
   * @return 0, if the song was added; -1, otherwise.
   *
   */
  int add_to_database();

  /**
   * Method that verifies if the database exists.
   * @return true, if the database exists; false,
   * otherwise.
   *
   */
  bool database_exists();

  /**
   * Method that creates the database (if it
   * does not exist).
   *
   */
  void create_database();

};
