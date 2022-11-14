/*
 * Copyright © 2022 Diego S.
 *
 */
#pragma once

#include <string>
#include <filesystem>
#include <gtkmm.h>
#include <sqlite3.h>

/**
 * Class to locate the .mp3 files
 * of the user.
 *
 */
class Miner {
 protected:
  /** The path of the directory where the music
      is located. */
  std::filesystem::path dir_path;
  /** The path of the database. */
  std::filesystem::path database_path;
  /** The name of the current user executing
      the application */
  std::string username;

  /** The list that holds song names and path */
  Glib::RefPtr<Gtk::ListStore> m_liststore;
  /** The database. */
  sqlite3 *db;
  /** The error message. */
  char *zErrMsg = 0;
  /** The return code. */
  int rc;
  /** The sql instruction */
  const char *sql;

 public:
    /** It holds the number of registered songs
      in the current directory */
  int counter;
  
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
   * Method that returns the current username.
   * @return the current username.
   *
   */
  std::string get_username();

  /**
   * Method that returns the list that holds
   * the song names and paths.
   * @return the list that holds the song
   * names and paths.
   *
   */
  Glib::RefPtr<Gtk::ListStore> get_liststore();

  /**
   * Method that defines the list that holds
   * the song names and paths.
   *
   */
  void set_liststore(Glib::RefPtr<Gtk::ListStore>& list);
  
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
