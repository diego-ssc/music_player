/*
 * Copyright © 2022 Diego S.
 *
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "Miner.h"
#include "Decoder.h"
#include <mpegfile.h>
#include <id3v2tag.h>
#include <sqlite3.h>
#include <stdio.h>
#include <unistd.h>
#include <filesystem>
#include <string>
#include <iostream>

Miner::Miner(std::string dir_path) {
  this->dir_path = dir_path;
  char name[1024];
  getlogin_r(name, sizeof(name));
  this->username = name;
  this->counter = 0;
}

std::filesystem::path Miner::get_dir_path() {
  return this->dir_path;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  int i;
  for (i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

bool has_suffix(const std::string &str, const std::string &suffix) {
  return str.size() >= suffix.size() &&
    str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void Miner::recursive_search() {
  for (const auto& dirEntry
         : std::filesystem::recursive_directory_iterator(dir_path)) {
    std::cout << dirEntry << std::endl;
    const std::string & str = dirEntry.path().string();
    if (dirEntry.is_directory()) continue;
    if (has_suffix(str, ".mp3")) {
      add_to_database(dirEntry.path());
      std::cout << "Fue agregado" << std::endl;
    }
  }
}

int Miner::add_to_database(std::filesystem::path song_path) {
  if (!database_exists())
    create_database();

  if (is_in_database("rolas", "path", song_path))
    return 0;

  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  const char *sql;
  Decoder decoder = Decoder((song_path).c_str());
  TagLib::ID3v2::Tag* tag = decoder.get_tag();
  rc = sqlite3_open(("/home/" + username + "/.local/share/"
                     "music_player/music.db").c_str(),
                    &db);

  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(1);
  } else {
    fprintf(stderr, "Opened database successfully\n");
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

    track = tag->track();
    year = tag->year();
    if (year == 0)
      year = 2022;

    genre = tag->genre();
    if (genre.isEmpty())
      genre = "Unknown";

    artist = tag->artist();
    if (artist.isEmpty())
      artist = "Unknown";

    album = tag->album();
    if (album.isEmpty())
      album = "Unknown";
  }

  std::string str_counter = std::to_string(counter);
  sql = ("INSERT INTO rolas (id_rola,id_performer,id_album,"            \
         "path,title,track,year,genre) "                                \
         "VALUES (" + str_counter + ", "                                \
         + str_counter + ", " + str_counter                             \
         + ", " + std::string(song_path) + ", " + title                 \
         + ", " + std::to_string(track) + ", "                          \
         + std::to_string(year) + ", " + genre                          \
         + ");").toCString();

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    fprintf(stdout, "[rolas] Records created successfully\n");
  }

  if (artist != "Unknown" &&
      is_in_database("performers", "name", artist.to8Bit())) {
    sql = ("INSERT INTO performers (id_performer,id_type,"              \
           "name) "                                                     \
           "VALUES (" + str_counter + ", "                              \
           "2, " + artist + ");").toCString();

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      fprintf(stdout, "[performers] Records created successfully\n");
    }
  }

  if (album != "Unknown" &&
      is_in_database("albums", "name", album.to8Bit())) {
    sql = ("INSERT INTO albums (id_album,path,"                       \
           "name,year) "                                              \
           "VALUES (" + str_counter + ", "                            \
           + std::string(song_path) + ", "                            \
           + album + std::to_string(year) + ");").toCString();

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      fprintf(stdout, "[albums] Records created successfully\n");
    }
  }

  counter++;
  delete tag;
  sqlite3_close(db);
  return 0;
}

bool Miner::is_in_database(std::string database, std::string field,
                           std::string data) {
  sqlite3 *db;
  int rc;
  const char *sql;

  rc = sqlite3_open(("/home/" + username + "/.local/share/"
                     "music_player/music.db").c_str(),
                    &db);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(1);
  } else {
    fprintf(stderr, "Opened database successfully\n");
  }

  sql = ("Select * from " + database + " where " +
         field + " = " + data).c_str();
  struct sqlite3_stmt *selectstmt;
  rc = sqlite3_prepare_v2(db, sql, -1, &selectstmt, NULL);
  bool a = (rc == SQLITE_OK) && (sqlite3_step(selectstmt) == SQLITE_ROW);
  sqlite3_finalize(selectstmt);

  return a;
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
  } else {
    fprintf(stderr, "Opened database successfully\n");
  }

  sql = "CREATE TABLE types("                 \
    "id_type INT PRIMARY KEY,"                \
    "description         TEXT);";

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    fprintf(stdout, "[types] Table created successfully\n");
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
  } else {
    fprintf(stdout, "[types] Records created successfully\n");
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
  } else {
    fprintf(stdout, "[performers] Table created successfully\n");
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
  } else {
    fprintf(stdout, "[persons] Table created successfully\n");
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
  } else {
    fprintf(stdout, "[groups] Table created successfully\n");
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
  } else {
    fprintf(stdout, "[albums] Table created successfully\n");
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
  } else {
    fprintf(stdout, "[rolas] Table created successfully\n");
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
  } else {
    fprintf(stdout, "[in_group] Table created successfully\n");
  }

  sqlite3_close(db);
}
