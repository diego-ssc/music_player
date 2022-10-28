/*
 * Copyright © 2022 Diego S.
 *
 */

#pragma once

#include <string>

/**
 * Class to locate the .mp3 files
 * of the user.
 *
 */
class Miner {
  /** The path of the directory where the music
      is located. */
  std::string dir_path;
  /** The path of the database. */
  std::string database_path;

 public:
  /**
   * Class constructor.
   * @param dir_path The path of the target directory.
   * @param database_path The path of the database.
   *
   */
  Miner(std::string dir_path, std::string database_path);

  /**
   * Returns the directory path.
   * @return the directory path.
   *
   */
  std::string get_dir_path();

  /**
   * Returns the database_path.
   * @return the database_path.
   *
   */
  std::string get_database_path();

  /**
   * Adds a song file to the database.
   * @param song_id The identifier of the song.
   * @return 0, if the song was added; -1, otherwise.
   *
   */
  int add_to_database(std::string song_id);

};
