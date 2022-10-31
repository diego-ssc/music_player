/*
 * Copyright © 2022 Diego S.
 *
 */
#include <iostream>
#include <filesystem>
#include <gst/gst.h>
#include <mpegfile.h>
#include <id3v2tag.h>
#include <glib.h>
#include "Decoder.h"

void usage() {
  std::cerr<<"Usage:"<<std::endl;
  std::cerr<<"./music_player [Music directory path]"<<std::endl;
}

int main(int argc, char** argv) {
  if (argc == 1) {
    usage();
    return 1;
  }

  gst_init (&argc, &argv);
  std::filesystem::path music_directory =
    argv[1];

  Decoder decoder = Decoder(argv[1]);
  TagLib::ID3v2::Tag* tag = decoder.get_tag();
  std::cout<<"Title: "<<tag->title()<<std::endl;
  std::cout<<"Artist: "<<tag->artist()<<std::endl;
  return 0;
}
