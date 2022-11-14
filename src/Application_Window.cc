/*
 * Copyright © 2022 Diego S.
 *
 */

#include "Application_Window.h"
#include "Miner.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <id3v2tag.h>
#include <tlist.h>
#include <id3v2frame.h>
#include <gdkmm/pixbufloader.h>
#include <attachedpictureframe.h>
#include <gtkmm/searchentry.h>
#include <list>
#include <chrono>
#include <thread>
#include <mutex>

std::mutex m_play;

Application_Window::Application_Window
(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
  : Gtk::ApplicationWindow(cobject),
    m_refBuilder(refBuilder),
    m_search(nullptr),
    m_searchbar(nullptr),
    m_searchentry(nullptr),
    m_treeview(nullptr),
    m_list(nullptr),
    m_add(nullptr),
    m_miner(nullptr),
    m_treeselection(nullptr),
    m_frame(nullptr),
    m_queries(nullptr),
    m_label(nullptr),
    m_decoder(""),
    m_file(nullptr),
    file(nullptr),
    m_prop_binding() {
  m_queries = new Database_Queries();

  m_search = m_refBuilder->get_widget<Gtk::ToggleButton>("toggle_search_button");
  if (!m_search)
    throw std::runtime_error
      ("No \"toggle_search_button\" object in music_player_window.ui");

  m_searchbar = m_refBuilder->get_widget<Gtk::SearchBar>("search_bar");
  if (!m_searchbar)
    throw std::runtime_error
      ("No \"search_bar\" object in music_player_window.ui");

  m_searchentry = m_refBuilder->get_widget<Gtk::Entry>("search_entry");
  if (!m_searchentry)
    throw std::runtime_error
      ("No \"search_entry\" object in music_player_window.ui");
  m_searchentry->set_placeholder_text("Search");

  m_searchentry->set_completion(m_queries->get_completion());

  m_treeview = m_refBuilder->get_widget<Gtk::TreeView>("song_list");
  if (!m_treeview)
    throw std::runtime_error
      ("No \"song_list\" object in music_player_window.ui");

  m_treeview->set_model(m_queries->get_liststore());
  m_queries->populate_list();
  m_queries->populate_completion();

  m_treeselection = m_treeview->get_selection();
  m_treeselection->set_mode(Gtk::SelectionMode::SINGLE);

  m_list = m_refBuilder->get_widget<Gtk::ToggleButton>("toggle_song_list");
  if (!m_list)
    throw std::runtime_error
      ("No \"toggle_song_list\" object in music_player_window.ui");

  m_add = m_refBuilder->get_widget<Gtk::ToggleButton>("toggle_add_directory");
  if (!m_add)
    throw std::runtime_error
      ("No \"toggle_add_directory\" object in music_player_window.ui");

  m_scrolledwindow = m_refBuilder->get_widget<Gtk::ScrolledWindow>("song_scroll");
  if (!m_scrolledwindow)
    throw std::runtime_error
      ("No \"m_scrolled_window\" object in music_player_window.ui");

  m_mediacontrols = m_refBuilder->get_widget<Gtk::MediaControls>("media_controls");
  if (!m_mediacontrols)
    throw std::runtime_error
      ("No \"media_controls\" object in music_player_window.ui");

  m_frame = m_refBuilder->get_widget<Gtk::AspectFrame>("song_image");
  if (!m_frame)
    throw std::runtime_error
      ("No \"song_image\" object in music_player_window.ui");

  m_label = m_refBuilder->get_widget<Gtk::Label>("song_playing_label");
  if (!m_label)
    throw std::runtime_error
      ("No \"song_playing_label\" object in music_player_window.ui");

  // BIND PROPERTIES

  /* Bidirectional binding, the toggle button is gonna be active whenever
    the search mode is enabled. If either property changes, the other is
    updated as well. */
  m_prop_binding = Glib::Binding::bind_property(m_search->property_active(),
                                                m_searchbar->property_search_mode_enabled(),
                                                Glib::Binding::Flags::BIDIRECTIONAL);

  m_prop_binding = Glib::Binding::bind_property(m_list->property_active(),
                                                m_scrolledwindow->property_visible(),
                                                Glib::Binding::Flags::BIDIRECTIONAL);

  // SIGNAL HANDLERS
  m_searchentry->signal_activate().connect // search
    (sigc::mem_fun(*this, &Application_Window::on_search_text_entered));

  m_add->signal_toggled().connect
    (sigc::mem_fun(*this, &Application_Window::on_toggle_add_directory));

  m_treeselection->signal_changed().connect
    (sigc::mem_fun(*this, &Application_Window::on_selection_changed));

  m_scrolledwindow->hide();
}

Application_Window* Application_Window::create() {
  auto refBuilder = Gtk::Builder::create_from_resource
    ("/mx/unam/ciencias/music_player/ui/music_player_window.ui");
  auto window = Gtk::Builder::get_widget_derived<Application_Window>
    (refBuilder, "MusicPlayerApplicationWindow");

  if (!window)
    throw std::runtime_error
      ("No \"MusicPlayerApplicationWindow\" object in music_player_window.iu");

  return window;
}

void Application_Window::create_miner(std::string path) {
  m_miner = new Miner(path);
  auto model = m_queries->get_liststore();
  m_miner->set_liststore(model);
  m_miner->recursive_search();
  m_queries->populate_completion();
  delete m_miner;
}

void Application_Window::play_song(std::string song_path,
                                   std::string song_name) {
  if (m_file != nullptr)
    delete m_file;
  Glib::RefPtr<Gtk::MediaStream> stream =
    m_mediacontrols->get_media_stream();
  if (stream != nullptr)
    stream->ended();

  m_decoder = Decoder(song_path.c_str());
  TagLib::ID3v2::Tag* tag = m_decoder.get_tag();
  TagLib::ID3v2::FrameList l = tag->frameListMap()["APIC"];
  TagLib::ID3v2::AttachedPictureFrame *pic_frame;
  void *src_image;
  FILE *jpeg_file;

  char name[1024];
  getlogin_r(name, sizeof(name));
  std::string username = name;

  std::filesystem::path path_str= "/home/" + username +
    "/.local/share/music_player/media";

  if (!std::filesystem::exists(path_str))
    std::filesystem::create_directory(path_str);

  path_str += "/cover.jpg";
  const char* path = path_str.c_str();
  jpeg_file = fopen(path, "wb");
  unsigned long size;

  if (!l.isEmpty()) {
    for(TagLib::ID3v2::FrameList::ConstIterator it = l.begin(); it != l.end(); ++it) {
      pic_frame = (TagLib::ID3v2::AttachedPictureFrame *)(*it) ;
      if (pic_frame->type() ==
          TagLib::ID3v2::AttachedPictureFrame::FrontCover) {
        size = pic_frame->picture().size();
        src_image = malloc(size);
        if (src_image) {
          memcpy(src_image, pic_frame->picture().data(), size);
          fwrite(src_image,size,1, jpeg_file);
          fclose(jpeg_file);
          free(src_image);
        }
      }
    }

    Gtk::Image image = Gtk::Image(path);
    image.set_size_request(800, 280);
    image.set_icon_size(Gtk::IconSize::NORMAL);
    m_frame->set_child(image);
  }
  m_label->set_text("Now Playing: " + song_name);
  if (file != nullptr) {
    file.reset(m_file);
    delete m_file;
  }
  file = Gtk::MediaFile::create_for_filename(song_path);

  m_mediacontrols->set_media_stream(file);
}

std::list<std::string>* parse_entry_flags(std::string &text,
                                          long unsigned int &flag_beginning) {
  int a = text.find("::", 0);
  flag_beginning = 0;
  if (a == -1)
    return nullptr;

  std::list<std::string>::iterator it;
  long unsigned int i,j;
  std::string str = text;
  std::string sub_str;
  std::list<std::string>* list = new std::list<std::string>();
  while (a != -1) {
    i = str.find_first_of("::");
    j = i;
    while(str.at(j) != ' ') {
      j--;
    }
    sub_str = str.substr(j + 1, (i-1)-j);
    if (flag_beginning == 0)
      flag_beginning = j + 1;

    j = i+2;

    if (j == str.size()) {
      a = str.find("::", i + 1);
      continue;
    }

    list->push_back(sub_str);
    while(str.at(j) != ' ') {
      j++;
      if (j == str.size())
        break;
    }
    list->push_back(str.substr(i + 2, (j)-(i+1)));

    a = str.find("::", i + 2);
  }
  return list;
}

std::string* parse_song_name(std::string &text,
                             long unsigned int flag_beginning) {
  if (text.empty())
    return nullptr;

  long unsigned int i = 0;
  while (i != text.size()) {
    i++;
    if (i == flag_beginning)
      break;
  }
  std::string *sub_str = new std::string(text.substr(0, i-1));
  return sub_str;
}

void Application_Window::on_search_text_entered() {
  std::string path;
  std::string text = m_searchentry->get_text();
  long unsigned int flag_beginning = 0;
  std::list<std::string> *flags = parse_entry_flags(text,
                                                    flag_beginning);
  std::string flag;
  std::string value;
  std::string *name;
  if (flag_beginning == 0) {
    name = new std::string(text);
    path = m_queries->get_path_from_title(*name);
  } else {
    name = parse_song_name(text, flag_beginning);
    if (name != nullptr) {
      path = m_queries->get_path_from_title(*name);
    }
  }

  if (flags != nullptr) {
    std::list<std::string>::iterator it;
    for (it = flags->begin(); it != flags->end(); ++it) {
      flag = *it;
      ++it;
      value = *it;
      m_queries->populate_list_by_flag(flag, value, *name);
    }
  }

  if (!path.empty())
    play_song(path, *name);

  m_searchentry->set_text("");

}

void Application_Window::on_toggle_add_directory() {
  if (!m_add->get_active())
    return;
  Gtk::FileChooserDialog* m_dialog = new Gtk::FileChooserDialog
    ("Please choose a folder", Gtk::FileChooser::Action::SELECT_FOLDER);

  m_dialog->set_transient_for(*this);
  m_dialog->set_modal(true);
  m_dialog->signal_response().connect
    (sigc::bind(sigc::mem_fun(*this, &Application_Window::
                              on_folder_dialog_response), m_dialog));

  m_dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
  m_dialog->add_button("_Open", Gtk::ResponseType::OK);

  m_dialog->show();
}

void Application_Window::on_folder_dialog_response
(int response_id, Gtk::FileChooserDialog* dialog) {
  switch (response_id) {
  case Gtk::ResponseType::OK: {
    dialog->hide();
    create_miner(dialog->get_file()->get_path());
    break;
  }
  default:
    break;
  }
  dialog->unset_transient_for();
  m_add->set_active(false);
  delete dialog;
}

void Application_Window::on_selection_changed() {
  auto iter = m_treeselection->get_selected();
  if (iter) {
    auto row = *iter;
    std::string path;
    std::string title;
    row.get_value(1, path);
    row.get_value(0, title);
    play_song(path, title);
  }
}
