/*
 * Copyright © 2022 Diego S.
 *
 */

#pragma once

#include <mpegfile.h>
#include <filesystem>
#include <gtkmm/mediafile.h>

/**
 * Class to play .mp3 files.
 *
 */
class Decoder {
  /** The song file */
  Glib::RefPtr<Gtk::MediaFile> song_file;

 public:
  /**
   * Class constructor
   *
   */
  explicit Decoder(const char* song_path);

  /**
   * Method that returns the song file.
   * @return the song file.
   *
   */
  Glib::RefPtr<Gtk::MediaFile> get_song_path();

  /**
   * Method that returns (if it has one) the
   * Id3v2 tag of the song file.
   * @return the Id3v2 tag of the song file
   * or NULL, if it has not one.
   *
   */
  TagLib::ID3v2::Tag* get_tag();
};
