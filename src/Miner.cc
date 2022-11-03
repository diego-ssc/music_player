/*
 * Copyright © 2022 Diego S.
 *
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "Miner.h"
#include <sqlite3.h>
#include <filesystem>
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

Miner::Miner(std::string dir_path) {
  this->dir_path = dir_path;
  this->username = getlogin();
}

std::filesystem::path Miner::get_dir_path() {
  return this->dir_path;
}

int Miner::add_to_database() {
  if (!database_exists())
    create_database();

  return 0;
}

bool Miner::database_exists() {
  std::filesystem::path p =
    "/home/" + username + "/.local/share/music_player/music.db";

  return std::filesystem::exists(p);
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  int i;
  for(i = 0; i<argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
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
      std::cerr<<"Could not create music database directory."<<std::endl;
    } else {
      std::cout<<"Music database directory created successfully"<<std::endl;
    }
  }

  rc = sqlite3_open(("/home/" + username + "/.local/share/music_player/music.db").c_str(),
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
