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
  /** It holds the number of registered songs
      in the current directory */
  int counter;

 public:
  /**
   * Class constructor.
   * @param dir_path The path of the target directory.
   *
   */
  explicit Miner(std::string dir_path);

  /**
   * Method that returns the directory path.
   * @return the directory path.
   *
   */
  std::filesystem::path get_dir_path();

  /**
   * Method that searches recursively for
   * .mp3 files in a certain directory.
   *
   */
  void recursive_search();
  
  /**
   * Method that adds a song file to the database.
   * @param song_path the path of the song that is being added.
   * @return 0, if the song was added; -1, otherwise.
   *
   */
  int add_to_database(std::filesystem::path song_path);

  /**
   * Method that verifies if the item that is being added
   * is already in the database.
   * @param database the database where the data will
   * be inserted.
   * @param field the field where the data will be inserted.
   * @param data the data that will be inserted.
   * @return true, if the song is already in the
   * database; false, otherwise.
   *
   */
  bool is_in_database(std::string database, std::string field,
                      std::string data);
  
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
