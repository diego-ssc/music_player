/*
 * Copyright © 2022 Diego S.
 *
 */
#pragma once

#include <gtkmm.h>

class Application_Window: public Gtk::ApplicationWindow {
 public:
  Application_Window(BaseObjectType* cobject,
                     const Glib::RefPtr<Gtk::Builder>& refBuilder);

  static Application_Window* create();

 protected:
  // Signal handlers
  void on_search_text_changed();

  Glib::RefPtr<Gtk::Builder> m_refBuilder;
  Gtk::ToggleButton* m_search;
  Gtk::SearchBar* m_searchbar;
  Gtk::SearchEntry* m_searchentry;
  Gtk::TreeView* m_treeview;
  Gtk::TreeModel* m_treemodel;
  Gtk::ToggleButton* m_list;
  Gtk::ScrolledWindow* m_scrolledwindow;
  Glib::RefPtr<Glib::Binding> m_prop_binding;
};
