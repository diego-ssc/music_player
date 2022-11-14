/*
 * Copyright © 2022 Diego S.
 *
 */
#pragma once

#include <gtkmm.h>
#include <filesystem>
#include <sqlite3.h>

class Database_Queries {
 protected:
  /** The list that holds song names and path */
  Glib::RefPtr<Gtk::ListStore> m_liststore;
  /** The name of the current user */
  std::string m_username;
  /** The entry completion object */
  Glib::RefPtr<Gtk::EntryCompletion> m_entrycompletion;
  /** The model of the entry completion object */
  Glib::RefPtr<Gtk::ListStore> m_completionmodel;
  /** The database. */
  sqlite3 *db;
  /** The error message. */
  char *zErrMsg = 0;
  /** The return code. */
  int rc;
  /** The sql instruction */
  const char *sql;
  /** The model of the completion record */
  class Completion_Record : public Gtk::TreeModel::ColumnRecord {
  public:
    Completion_Record() {
      add(col_text);
      add(col_text2);
    }
    Gtk::TreeModelColumn<Glib::ustring> col_text;
    Gtk::TreeModelColumn<Glib::ustring> col_text2;
  };

  Completion_Record m_completionrecord;

 public:
  /**
   * Class constructor.
   *
   */
  Database_Queries();

  /**
   * Method that returns the data list.
   * @return the song names list.
   *
   */
  Glib::RefPtr<Gtk::ListStore> get_liststore();

  /**
   * Method that returns the completion
   * of the entry.
   * @return the entry completion.
   *
   */
  Glib::RefPtr<Gtk::EntryCompletion> get_completion();

  /**
   * Method that populates the list
   * linked to the song list.
   *
   */
  void populate_list();

  /**
   * Method that populates the list
   * linked to the completion entry.
   *
   */
  void populate_completion();

  /**
   * Method that verifies that every single
   * song stored in the database exists.
   * If it no longer exists, it will delete
   * it from the database.
   *
   */
  void refresh_database();

  /**
   * Method that returns the path assocciated
   * with the given title.
   * @param title the song title.
   * @return the song path.
   *
   */
  std::string get_path_from_title(std::string title);


  /**
   * Method that populates the liststore
   * with the flags.
   * @param flag the flag.
   * @param value the flag value.
   * @param title the song title.
   * @return the song that fullfills the
   * paramaters.
   *
   */
  std::string populate_list_by_flag(std::string flag,
                             std::string value,
                             std::string title);

  /**
   * Method that populates the liststore
   * with the flags.
   * @param flag the flag.
   * @param value the flag value.
   * @param title the song title.
   * @return the song that fullfills the
   * paramaters.
   *
   */
  std::string populate_list_by_flag(std::string flag,
                                    int value,
                                    std::string title);
};
