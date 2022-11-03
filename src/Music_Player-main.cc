/*
 * Copyright © 2022 Diego S.
 *
 */
#include <gst/gst.h>
#include <mpegfile.h>
#include <id3v2tag.h>
#include <glib.h>
#include <iostream>
#include <filesystem>
#include "Decoder.h"
#include "Miner.h"

void usage() {
  std::cerr << "Usage:" << std::endl;
  std::cerr << "./music_player [Music directory path]" << std::endl;
}

int main(int argc, char** argv) {
  if (argc == 1) {
    usage();
    return 1;
  }

  gst_init(&argc, &argv);
  std::filesystem::path music_directory =
    argv[1];

  Decoder decoder = Decoder(argv[1]);

  // decoder.play();
  Miner miner = Miner("hello");
  miner.add_to_database();
  TagLib::ID3v2::Tag* tag = decoder.get_tag();
  if (tag == nullptr) return 0;
  std::cout << "Artist: " << tag->artist() << std::endl;
  std::cout << "Title: " << tag->title() << std::endl;
  std::cout << "Album: "<<tag->album() << std::endl;
  std::cout << "Year: " << tag->year() << std::endl;
  std::cout << "Genre: " << tag->genre() << std::endl;
  std::cout << "Track number: " << tag->track() << std::endl;
  delete tag;
  return 0;
}
