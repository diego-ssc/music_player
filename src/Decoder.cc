/*
 * Copyright © 2022 Diego S.
 *
 */
#include <mpegfile.h>
#include <gst/gst.h>
#include <glib.h>
#include "Decoder.h"

Decoder::Decoder(const char* song_path) {
  this->song_path = song_path;
}

const char* Decoder::get_song_path() {
  return this->song_path;
}

int Decoder::play() {
  GstBus *bus;
  GstMessage *msg;
  GstElement *src, *decoder, *sink;
  GstElement *pipeline;

  pipeline = gst_pipeline_new("pipeline");
  src      = gst_element_factory_make("filesrc", "source");
  decoder  = gst_element_factory_make("decodebin2", "decoder");
  sink = gst_element_factory_make("autoaudiosink", "play_audio");

  if (!pipeline || !src || !decoder || !sink) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }

  g_object_set(G_OBJECT(src), "location", song_path, NULL);
  gst_bin_add_many(GST_BIN(pipeline), src, decoder, sink, NULL);

  gst_element_link_many(src, decoder, sink, NULL);

  gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);


  bus = gst_element_get_bus (pipeline);
  // gst_bus_have_pending (GstBus * bus);
  msg =
    gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
				GST_MESSAGE_EOS);

  if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR) {
    g_error ("An error occurred! Re-run with the GST_DEBUG=*:WARN environment "
	     "variable set for more details.");
  }

  gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);

  /* un-referencing all the elements in the pipeline */
  gst_object_unref(GST_OBJECT(pipeline));
  gst_object_unref(GST_OBJECT(bus));
  gst_object_unref(GST_OBJECT(msg));

  return 0;
}

TagLib::ID3v2::Tag* Decoder::get_tag() {
  TagLib::MPEG::File file = TagLib::MPEG::File(song_path, true,
			   TagLib::AudioProperties::ReadStyle::Accurate);

  if (!file.hasID3v2Tag())
    return NULL;

  return file.ID3v2Tag();
}
