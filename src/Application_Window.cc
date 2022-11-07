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
    m_treemodel(nullptr) {
  // m_search = m_refBuilder->get_widget<Gtk::ToggleButton>
  
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

