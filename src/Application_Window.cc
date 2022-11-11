/*
 * Copyright © 2022 Diego S.
 *
 */

#include "Application_Window.h"
#include "Miner.h"
#include <stdexcept>
#include <iostream>

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
    m_prop_binding() {
  m_search = m_refBuilder->get_widget<Gtk::ToggleButton>("toggle_search_button");
  if (!m_search)
    throw std::runtime_error
      ("No \"toggle_search_button\" object in music_player_window.ui");

  m_searchbar = m_refBuilder->get_widget<Gtk::SearchBar>("search_bar");
  if (!m_searchbar)
    throw std::runtime_error
      ("No \"search_bar\" object in music_player_window.ui");

  m_searchentry = m_refBuilder->get_widget<Gtk::SearchEntry>("search_entry");
  if (!m_searchentry)
    throw std::runtime_error
      ("No \"search_entry\" object in music_player_window.ui");

  m_treeview = m_refBuilder->get_widget<Gtk::TreeView>("song_list");
  if (!m_treeview)
    throw std::runtime_error
      ("No \"song_list\" object in music_player_window.ui");

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
  m_searchentry->signal_search_changed().connect
    (sigc::mem_fun(*this, &Application_Window::on_search_text_changed));

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
  m_miner->recursive_search();
  m_treeview->set_model(m_miner->get_liststore());
  delete m_miner;
}

void Application_Window::on_search_text_changed() {
  const auto text = m_searchentry->get_text();
  if (text.empty())
    return;
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
    std::string value;
    row.get_value(1, value);
    Glib::RefPtr<Gtk::MediaFile> file =
      Gtk::MediaFile::create_for_filename(value);
    m_mediacontrols->set_media_stream(file);
  }
}

