/*
 * Copyright © 2022 Diego S.
 *
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "Miner.h"
#include "Decoder.h"
#include "Model_Columns.h"
#include <mpegfile.h>
#include <id3v2tag.h>
#include <sqlite3.h>
#include <stdio.h>
#include <unistd.h>
#include <gtkmm.h>
#include <cstring>
#include <filesystem>
#include <string>
#include <iostream>

char callback_result[1024] = "";

Miner::Miner(std::string dir_path) {
  this->dir_path = dir_path;
  char name[1024];
  getlogin_r(name, sizeof(name));
  this->username = name;
  this->counter = 0;
  this->m_liststore = Gtk::ListStore::create(m_Columns);
}

std::filesystem::path Miner::get_dir_path() {
  return this->dir_path;
}

Glib::RefPtr<Gtk::ListStore> Miner::get_liststore() {
  return this->m_liststore;
}

/*
 * When the data in the field does not exist (using SELECT operator),
 * it won't execute; that's why it's cleared before the execution of
 * sqlite3_exec.
 */
static int callback(void *userData, int numCol,
                    char **colData, char **colName) {
  int i;
  for (i = 0; i < numCol; i++) {
    strcat(callback_result, colData[i]);
  }
  return 0;
}

static int id_reference(void *userData, int numCol,
                        char **colData, char **colName) {
  strcat(callback_result, colData[0]);
  return 0;
}

bool has_suffix(const std::string &str, const std::string &suffix) {
  return str.size() >= suffix.size() &&
    str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void Miner::recursive_search() {
  for (const auto& dirEntry
         : std::filesystem::recursive_directory_iterator(dir_path)) {
    const std::string & str = dirEntry.path().string();
    if (dirEntry.is_directory()) continue;
    if (has_suffix(str, ".mp3")) {
      std::filesystem::path song_path = std::filesystem::absolute(dirEntry.path());
      add_to_database(song_path);
      add_to_list(song_path);
    }
  }
}

void replace_single_quotation_marks(TagLib::String &string) {
  int i = string.find("'", 0);
  while (i != -1) {
    string = (string.to8Bit()).replace(i, 1, "''");
    i = string.find("'", i + 2);
  }
}

int Miner::add_to_database(std::filesystem::path path) {
  if (!database_exists())
    create_database();

  TagLib::String song_path = TagLib::String(path);
  replace_single_quotation_marks(song_path);
  if (is_in_database("rolas", "path", song_path.to8Bit()))
    return 0;

  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  const char *sql;
  Decoder decoder = Decoder(path.c_str());
  TagLib::ID3v2::Tag* tag = decoder.get_tag();
  rc = sqlite3_open(("/home/" + username + "/.local/share/"
                     "music_player/music.db").c_str(),
                    &db);

  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(1);
  }

  sql = "PRAGMA foreign_keys = ON;";
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  TagLib::String title;
  TagLib::String genre;
  TagLib::String artist;
  TagLib::String album;
  int track;
  int year;

  if (tag == nullptr) {
    title = "Unknown";
    track = 0;
    year = 2022;
    genre = "Unknown";
    artist = "Unknown";
    album = "Unknown";
  } else {
    title = tag->title();
    if (title.isEmpty())
      title = "Unknown";
    replace_single_quotation_marks(title);

    track = tag->track();
    year = tag->year();
    if (year == 0)
      year = 2022;

    genre = tag->genre();
    if (genre.isEmpty())
      genre = "Unknown";
    replace_single_quotation_marks(genre);

    artist = tag->artist();
    if (artist.isEmpty())
      artist = "Unknown";
    replace_single_quotation_marks(artist);

    album = tag->album();
    if (album.isEmpty())
      album = "Unknown";
    replace_single_quotation_marks(album);
  }

  std::string str_counter = std::to_string(counter);
  TagLib::String str = "INSERT INTO rolas (id_rola,id_performer,"       \
    "id_album,path,title,track,year,genre) "                            \
    "VALUES (" + str_counter + ", "                                     \
    "NULL, NULL"                                                        \
    ", '" + song_path + "', '" + title                                  \
    + "', " + std::to_string(track) + ", "                              \
    + std::to_string(year) + ", '" + genre                              \
    + "');";
  sql = str.toCString(true);
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    fprintf(stderr, "%s\n", sql);
    sqlite3_free(zErrMsg);
  }

  if (artist != "Unknown" &&
      !is_in_database("performers", "name", artist.to8Bit())) {
    str = "INSERT INTO performers (id_performer,id_type,"               \
      "name) "                                                          \
      "VALUES (" + str_counter + ", "                                   \
      "2, '" + artist + "');";
    sql = str.toCString(true);
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      fprintf(stderr, "%s\n", sql);
      sqlite3_free(zErrMsg);
    }
  }

  if (album != "Unknown" &&
      !is_in_database("albums", "name", album.to8Bit())) {
    str = ("INSERT INTO albums (id_album,path,"                        \
           "name,year) "                                               \
           "VALUES (" + str_counter + ", '"                            \
           + song_path + "', '"                                        \
           + album + "', " + std::to_string(year) + ");").toCString();
    sql = str.toCString(true);
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      fprintf(stderr, "%s\n", sql);
      sqlite3_free(zErrMsg);
    }
  }

  if (artist != "Unknown") {
    if (strlen(callback_result) != 0)
      memset(&callback_result[0], 0, sizeof(callback_result));

    str = "SELECT id_performer FROM performers"
      " WHERE EXISTS (SELECT name FROM performers"
      " WHERE name = '" + artist +"');";
    sql = str.toCString(true);
    rc = sqlite3_exec(db, sql, id_reference, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      fprintf(stderr, "%s\n", sql);
      sqlite3_free(zErrMsg);
    }

    str = "UPDATE rolas set id_performer = " + std::string(callback_result) +
      " where id_rola = " + str_counter;
    sql = str.toCString(true);
    rc = sqlite3_exec(db, sql, id_reference, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      fprintf(stderr, "%s\n", sql);
      sqlite3_free(zErrMsg);
    }
  }

    if (album != "Unknown") {
    if (strlen(callback_result) != 0)
      memset(&callback_result[0], 0, sizeof(callback_result));

    str = "SELECT id_album FROM albums"
      " WHERE EXISTS (SELECT name FROM albums"
      " WHERE name = '" + album +"');";
    sql = str.toCString(true);
    rc = sqlite3_exec(db, sql, id_reference, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      fprintf(stderr, "%s\n", sql);
      sqlite3_free(zErrMsg);
    }

    str = "UPDATE rolas set id_album = " + std::string(callback_result) +
      " where id_rola = " + str_counter;
    sql = str.toCString(true);
    rc = sqlite3_exec(db, sql, id_reference, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      fprintf(stderr, "%s\n", sql);
      sqlite3_free(zErrMsg);
    }
  }

  counter++;
  delete tag;
  sqlite3_close(db);
  return 0;
}

int Miner::add_to_list(std::filesystem::path path) {
  auto iter = m_liststore->append();
  auto row = *iter;
  Decoder decoder = Decoder(path.c_str());
  TagLib::ID3v2::Tag* tag = decoder.get_tag();
  row[m_Columns.m_col_text] = tag->title().to8Bit();
  row[m_Columns.m_col_path] = std::string(path);
  return 0;
}

/** Only for 'TEXT' data */
bool Miner::is_in_database(std::string database, std::string field,
                           std::string data) {
  sqlite3 *db;
  int rc;
  const char *sql;
  char *zErrMsg = 0;

  rc = sqlite3_open(("/home/" + username + "/.local/share/"
                     "music_player/music.db").c_str(),
                    &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(1);
  }
  std::string str = "SELECT " + field + " FROM " + database +
    " WHERE EXISTS (SELECT " + field + " FROM " + database +
    " WHERE " + field + " = '" + data +"');";

  sql = str.c_str();
  /* callback_result cleared before callback
     because callback won't execute if the
     SELECT instruction does not return anything */
  if (strlen(callback_result) != 0) {
    memset(&callback_result[0], 0, sizeof(callback_result));
  }
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    fprintf(stderr, "%s\n", sql);
    sqlite3_free(zErrMsg);
  }
  sqlite3_close(db);
  return strlen(callback_result) != 0;
}

bool Miner::database_exists() {
  std::filesystem::path p =
    "/home/" + username + "/.local/share/music_player/music.db";

  return std::filesystem::exists(p);
}

void Miner::create_database() {
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  const char *sql;
  std::filesystem::path p =
    "/home/"+ username +"/.local/share/music_player";

  if (!std::filesystem::exists(p)) {
    if (!std::filesystem::create_directory(p)) {
      std::cerr << "Could not create music database directory." << std::endl;
    } else {
      std::cout << "Music database directory created successfully" << std::endl;
    }
  }

  rc = sqlite3_open(("/home/" + username + "/.local/share/"
                     "music_player/music.db").c_str(),
                    &db);

  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(1);
  }

  sql = "CREATE TABLE types("                 \
    "id_type INT PRIMARY KEY,"                \
    "description         TEXT);";

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  sql = "INSERT INTO types (id_type,description) "              \
    "VALUES (0, 'Person'); "                                    \
    "INSERT INTO types (id_type,description) "                  \
    "VALUES (1, 'Group'); "                                     \
    "INSERT INTO types (id_type,description)"                   \
    "VALUES (2, 'Unknown');";

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  sql = "CREATE TABLE performers("                              \
    "id_performer INT PRIMARY KEY,"                             \
    "id_type      INTEGER,"                                     \
    "name         TEXT,"                                        \
    "FOREIGN KEY (id_type) REFERENCES types(id_type));";

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  sql = "CREATE TABLE persons("                                 \
    "id_person  INT PRIMARY KEY,"                               \
    "stage_name TEXT,"                                          \
    "real_name  TEXT,"                                          \
    "birth_date TEXT,"                                          \
    "death_date TEXT);";

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  sql = "CREATE TABLE groups("                                         \
    "id_group   INT PRIMARY KEY,"                                      \
    "name       TEXT,"                                                 \
    "start_date TEXT,"                                                 \
    "end_date   TEXT);";

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  sql = "CREATE TABLE albums("                                       \
    "id_album INT PRIMARY KEY,"                                      \
    "path     TEXT,"                                                 \
    "name     TEXT,"                                                 \
    "year     INTEGER);";

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  sql = "CREATE TABLE rolas("                                           \
    "id_rola      INT PRIMARY KEY,"                                     \
    "id_performer INTEGER,"                                             \
    "id_album     INTEGER,"                                             \
    "path         TEXT,"                                                \
    "title        TEXT,"                                                \
    "track        INTEGER,"                                             \
    "year         INTEGER,"                                             \
    "genre        TEXT,"                                                \
    "FOREIGN KEY (id_performer) REFERENCES performers(id_performer),"   \
    "FOREIGN KEY (id_album) REFERENCES albums(id_album));";

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  sql = "CREATE TABLE in_group("                                        \
    "id_person    INTEGER,"                                             \
    "id_group     INTEGER,"                                             \
    "PRIMARY KEY (id_person, id_group),"                                \
    "FOREIGN KEY (id_person) REFERENCES persons(id_person),"            \
    "FOREIGN KEY (id_group)  REFERENCES groups(id_group));";

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  sqlite3_close(db);
}
