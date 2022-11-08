/*
 * Copyright © 2022 Diego S.
 * Main class of the music player.
 *
 */
#include <gst/gst.h>
#include "Application.h"
#include "Miner.h"

int main(int argc, char** argv) {
  auto application = Application::create();
  gst_init(&argc, &argv);

  return application->run(argc, argv);
}
