/*
 * Copyright © 2022 Diego S.
 *
 */
#pragma once

#include <gtkmm.h>
#include "Miner.h"

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
  Glib::RefPtr<Gtk::TreeModel> m_treemodel;
  Gtk::ToggleButton* m_list;
  Gtk::ToggleButton* m_add;
  Gtk::ScrolledWindow* m_scrolledwindow;
  Gtk::MediaControls* m_mediacontrols;
  Gtk::TreeViewColumn* m_treecolumn;
  Glib::RefPtr<Miner> m_miner;
  Glib::RefPtr<Glib::Binding> m_prop_binding;
};
