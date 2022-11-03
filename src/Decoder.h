/*
 * Copyright © 2022 Diego S.
 *
 */

#pragma once

#include <mpegfile.h>
#include <filesystem>

/**
 * Class to play .mp3 files.
 *
 */
class Decoder {
  /** The path of the .mp3 song */
  const char* song_path;

 public:
  /**
   * Class constructor
   *
   */
  explicit Decoder(const char* song_path);

  /**
   * Method that returns the path of the song.
   * @return the path of the song;
   *
   */
  const char* get_song_path();

  /**
   * Method that plays th requested song
   * by its path in the filesystem.
   * @return 0, if the song could be processed;
   * 0, otherwise.
   *
   */
  int play();

  /**
   * Method that returns (if it has one) the
   * Id3v2 tag of the song file.
   * @return the Id3v2 tag of the song file
   * or NULL, if it has not one.
   *
   */
  TagLib::ID3v2::Tag* get_tag();
};
