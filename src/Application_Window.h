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
  void on_toggle_add_directory();
  void on_selection_changed();
  void on_folder_dialog_response
  (int response_id, Gtk::FileChooserDialog* dialog);

  void create_miner(std::string path);
  
  Glib::RefPtr<Gtk::Builder> m_refBuilder;
  Gtk::ToggleButton* m_search;
  Gtk::SearchBar* m_searchbar;
  Gtk::SearchEntry* m_searchentry;
  Gtk::TreeView* m_treeview;
  Gtk::ToggleButton* m_list;
  Gtk::ToggleButton* m_add;
  Gtk::ScrolledWindow* m_scrolledwindow;
  Gtk::MediaControls* m_mediacontrols;
  Miner* m_miner;
  Glib::RefPtr<Gtk::TreeSelection> m_treeselection;
  Gtk::AspectFrame* m_frame;
  Glib::RefPtr<Glib::Binding> m_prop_binding;
};
        
