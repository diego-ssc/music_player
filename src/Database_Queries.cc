/*
 * Copyright © 2022 Diego S.
 *
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "Database_Queries.h"
#include "Model_Columns.h"
#include <iostream>

std::string callback_str = "";

Database_Queries::Database_Queries()
  : m_liststore(nullptr),
    m_entrycompletion(nullptr),
    m_completionmodel(nullptr) {
  m_liststore = Gtk::ListStore::create(m_Columns);
  char name[1024];
  getlogin_r(name, sizeof(name));
  m_username = name;
  m_entrycompletion = Gtk::EntryCompletion::create();
  m_completionmodel = Gtk::ListStore::create(m_completionrecord);

  m_entrycompletion->set_model(m_completionmodel);
  m_entrycompletion->set_text_column(m_completionrecord.col_text);
  m_entrycompletion->set_minimum_key_length(1);
  m_entrycompletion->set_popup_completion(true);
}

Glib::RefPtr<Gtk::ListStore> Database_Queries::get_liststore() {
  return this->m_liststore;
}

Glib::RefPtr<Gtk::EntryCompletion> Database_Queries::get_completion() {
  return this->m_entrycompletion;
}

static int callback(void *userData, int numCol,
                    char **colData, char **colName) {
  int i;
  for (i = 0; i < numCol; i++) {
    callback_str += colData[i];
    callback_str += "\n";
  }
  return 0;
}

void Database_Queries::refresh_database() {
  rc = sqlite3_open(("/home/" + m_username + "/.local/share/"
                     "music_player/music.db").c_str(),
                    &db);

  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(1);
  }

  sql = "SELECT title from rolas;";
  if (!callback_str.empty())
    callback_str.clear();

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  std::string str = callback_str;
  std::string sub_str;
  int i;

  while (!str.empty()) {
    i = str.find_first_of("\n", 0);
    sub_str = str.substr(0, i);
    str.erase(0, i + 1);

    std::filesystem::path p = sub_str;
    if (std::filesystem::exists(p))
      continue;

    sub_str = "DELETE from rolas where path = '"
      + sub_str + "';";
    sql = sub_str.c_str();

    if (!callback_str.empty())
      callback_str.clear();

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
  }
}

void replace_single_quotation_marks(std::string &string) {
  int i = string.find("'", 0);
  while (i != -1) {
    string = string.replace(i, 1, "''");
    i = string.find("'", i + 2);
  }
}

bool isWhitespace(std::string s) {
  for(long unsigned int index = 0; index < s.length(); index++){
    if(!std::isspace(s[index]))
      return false;
  }
  return true;
}

bool model_is_empty(std::shared_ptr<Gtk::ListStore> model) {
  auto iter = model->get_iter("0:0");
  std::string path;
  std::string title;
  if (iter) {
    auto row = *iter;
    row.get_value(1, path);
    row.get_value(0, title);
  }
  return isWhitespace(path) && isWhitespace(title);
}

void Database_Queries::populate_list() {
  if (!model_is_empty(m_liststore))
    m_liststore = Gtk::ListStore::create(m_Columns);


  std::filesystem::path path = "/home/" + m_username + "/.local/share/"
    "music_player/music.db";
  if (!std::filesystem::exists(path))
    return;

  rc = sqlite3_open(("/home/" + m_username + "/.local/share/"
                     "music_player/music.db").c_str(),
                    &db);

  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(1);
  }

  sql = "SELECT title FROM rolas;";

  if (!callback_str.empty())
    callback_str.clear();

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  int i;
  std::string str = callback_str;
  std::string sub_str;

  while (!str.empty()) {
    i = str.find_first_of("\n", 0);
    auto iter = m_liststore->append();
    auto row = *iter;
    sub_str = str.substr(0, i);
    str.erase(0, i + 1);
    row[m_Columns.m_col_name] = sub_str;

    replace_single_quotation_marks(sub_str);
    sub_str = "SELECT path from rolas WHERE"
      " title = '" + sub_str + "';";
    sql = sub_str.c_str();
    if (!callback_str.empty())
      callback_str.clear();

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }

    i = callback_str.find_first_of("\n", 0);

    callback_str.erase(i, i + 1);
    row[m_Columns.m_col_path] = callback_str;
  }

  sqlite3_close(db);
}

void Database_Queries::populate_completion() {
  if (!model_is_empty(m_completionmodel))
    m_completionmodel = Gtk::ListStore::create(m_completionrecord);

  std::filesystem::path path = "/home/" + m_username + "/.local/share/"
    "music_player/music.db";
  if (!std::filesystem::exists(path))
    return;

  rc = sqlite3_open(("/home/" + m_username + "/.local/share/"
                     "music_player/music.db").c_str(),
                    &db);

  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(1);
  }

  sql = "SELECT title FROM rolas;";

  if (!callback_str.empty())
    callback_str.clear();

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  int i;
  std::string str = callback_str;
  std::string sub_str;

  while (!str.empty()) {
    i = str.find_first_of("\n", 0);
    Gtk::TreeModel::Row row = *(m_completionmodel->append());

    sub_str = str.substr(0, i);
    str.erase(0, i + 1);
    row[m_completionrecord.col_text] = sub_str;
  }
}

std::string Database_Queries::get_path_from_title(std::string title) {
  rc = sqlite3_open(("/home/" + m_username + "/.local/share/"
                     "music_player/music.db").c_str(),
                    &db);

  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(1);
  }

  std::string str = "SELECT path FROM rolas WHERE title = '" + title + "';";
  sql = str.c_str();

  if (!callback_str.empty())
    callback_str.clear();

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  str = callback_str;
  std::string sub_str;
  int i = str.find_first_of("\n", 0);
  return sub_str = str.substr(0, i);
}

std::string Database_Queries::populate_list_by_flag(std::string flag,
                                             std::string value,
                                             std::string title) {
  rc = sqlite3_open(("/home/" + m_username + "/.local/share/"
                     "music_player/music.db").c_str(),
                    &db);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(1);
  }

  std::string str = "select title from rolas where " + flag +
    " = '" + value + "' and title = '" + title + "';";
  sql = str.c_str();

  if (!callback_str.empty())
    callback_str.clear();

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  int i;
  str = callback_str;
  std::string sub_str;

  i = str.find_first_of("\n", 0);
  sub_str = str.substr(0, i);
  sqlite3_close(db);

  return sub_str;
}

std::string Database_Queries::populate_list_by_flag(std::string flag,
                                             int value,
                                             std::string title) {
  rc = sqlite3_open(("/home/" + m_username + "/.local/share/"
                     "music_player/music.db").c_str(),
                    &db);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(1);
  }

  std::string str = "select title from rolas where " + flag +
    " = " + std::to_string(value) + " and title = '" + title + "';";
  sql = str.c_str();

  if (!callback_str.empty())
    callback_str.clear();

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  int i;
  str = callback_str;
  std::string sub_str;

  i = str.find_first_of("\n", 0);
  sub_str = str.substr(0, i);
  sqlite3_close(db);

  return sub_str;
}
