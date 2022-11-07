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

Decoder::Decoder(const char* song_path) {
  this->song_path = song_path;
}

const char* Decoder::get_song_path() {
  return this->song_path;
}

void on_pad_added(GstElement *element, GstPad *pad,
                  gpointer data) {
  GstPad *sinkpad;
  GstElement *queue = reinterpret_cast<GstElement *>(data);
  g_print("Dynamic pad created, linking demuxer/decoder\n");
  sinkpad = gst_element_get_static_pad(queue, "sink");
  gst_pad_link(pad, sinkpad);
  gst_object_unref(sinkpad);
}

int Decoder::play() {
  GstBus *bus;
  GstMessage *msg;
  GstElement *filesrc, *mpegaudioparse, *mpg123audiodec,
    *audioconvert, *audioresample, *autoaudiosink;
  GstElement *pipeline;

  pipeline        = gst_pipeline_new("pipeline");
  filesrc         = gst_element_factory_make("filesrc", "src");
  mpegaudioparse  = gst_element_factory_make("mpegaudioparse", "parse");
  mpg123audiodec  = gst_element_factory_make("mpg123audiodec", "audiodec");
  audioconvert    = gst_element_factory_make("audioconvert", "audioconvert");
  audioresample   = gst_element_factory_make("audioresample", "audioresample");
  autoaudiosink   = gst_element_factory_make("autoaudiosink", "autoaudiosink");

  if (!pipeline || !filesrc || !mpegaudioparse || !mpg123audiodec ||
      !audioconvert || !audioresample || !autoaudiosink) {
    g_printerr("Not all elements could be created.\n");
    return -1;
  }

  g_object_set(filesrc, "location", this->song_path, NULL);
  gst_bin_add_many(GST_BIN(pipeline), filesrc, mpegaudioparse, mpg123audiodec,
                   audioconvert, audioresample, autoaudiosink, NULL);
  if (!gst_element_link_many(filesrc, mpegaudioparse, mpg123audiodec,
                             audioconvert, audioresample, autoaudiosink,
                             NULL)) {
    g_warning("Failed to link elements.\n");
    exit(1);
  }


  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  bus = gst_element_get_bus(pipeline);
  msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)
                                   (GST_MESSAGE_EOS | GST_MESSAGE_ERROR));

  switch (GST_MESSAGE_TYPE(msg)) {
  case GST_MESSAGE_EOS: {
    g_print("EOS\n");
    break;
  }
  case GST_MESSAGE_ERROR: {
    GError* err = NULL;
    gchar* dbg = NULL;

    gst_message_parse_error(msg, &err, &dbg);
    if (err) {
      g_printerr("ERROR: %s\n", err->message);
      g_error_free(err);
    }
    if (dbg) {
      g_printerr("[Debug details: %s]\n", dbg);
      g_free(dbg);
    }
    break;
  }
  default:
    g_printerr("Unexpected message of type %d", GST_MESSAGE_TYPE(msg));
    break;
  }

  gst_message_unref(msg);
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);
  gst_object_unref(bus);

  return 0;
}

TagLib::ID3v2::Tag* Decoder::get_tag() {
  TagLib::MPEG::File* file =
    new TagLib::MPEG::File(this->song_path, true,
                           TagLib::AudioProperties::ReadStyle::Accurate);

  if (!file->hasID3v2Tag())
    return NULL;

  return file->ID3v2Tag();
}
