/*
 * Copyright © 2022 Diego S.
 *
 */

#include "Application_Window.h"
#include <stdexcept>

Application_Window::Application_Window
(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
  : Gtk::ApplicationWindow(cobject),
    m_refBuilder(refBuilder),
    m_search(nullptr),
    m_searchbar(nullptr),
    m_searchentry(nullptr),
    m_treeview(nullptr),
    m_treemodel(nullptr),
    m_list(nullptr),
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

  // m_treemodel // Get from tree_view

  m_list = m_refBuilder->get_widget<Gtk::ToggleButton>("toggle_song_list");
  if (!m_list)
    throw std::runtime_error
      ("No \"toggle_song_list\" object in music_player_window.ui");

  m_scrolledwindow = m_refBuilder->get_widget<Gtk::ScrolledWindow>("song_scroll");
  if (!m_scrolledwindow)
    throw std::runtime_error
      ("No \"m_scrolled_window\" object in music_player_window.ui");

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

void Application_Window::on_search_text_changed() {
  const auto text = m_searchentry->get_text();
  if (text.empty())
    return;
}
