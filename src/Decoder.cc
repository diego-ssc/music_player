/*
 * Copyright © 2022 Diego S.
 *
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <mpegfile.h>
#include <id3v2tag.h>
#include <gst/gst.h>
#include <glib.h>
#include "Decoder.h"
#include <giomm/file.h>

Decoder::Decoder(const char* song_path) {
  this->song_file = Gtk::MediaFile::create_for_filename(song_path);
}

Glib::RefPtr<Gtk::MediaFile> Decoder::get_song_path() {
  return this->song_file;
}

TagLib::ID3v2::Tag* Decoder::get_tag() {
  TagLib::MPEG::File* file =
    new TagLib::MPEG::File(this->song_file->get_file()->get_path().c_str(), true,
                           TagLib::AudioProperties::ReadStyle::Accurate);

  if (!file->hasID3v2Tag())
    return NULL;

  return file->ID3v2Tag();
}
