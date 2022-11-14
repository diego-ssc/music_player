/*
 * Copyright © 2022 Diego S.
 *
 */
#pragma once

#include <gtkmm.h>
#include <string>
#include "Miner.h"
#include "Database_Queries.h"
#include "Decoder.h"

class Application_Window: public Gtk::ApplicationWindow {
 public:
  Application_Window(BaseObjectType* cobject,
                     const Glib::RefPtr<Gtk::Builder>& refBuilder);

  static Application_Window* create();

 protected:
  // Signal handlers
  void on_search_text_entered();
  void on_toggle_add_directory();
  void on_selection_changed();
  void on_folder_dialog_response
  (int response_id, Gtk::FileChooserDialog* dialog);
  
  void create_miner(std::string path);
  void play_song(std::string song_path,
                 std::string song_name);
  
  Glib::RefPtr<Gtk::Builder> m_refBuilder;
  Gtk::ToggleButton* m_search;
  Gtk::SearchBar* m_searchbar;
  Gtk::Entry* m_searchentry;
  Gtk::TreeView* m_treeview;
  Gtk::ToggleButton* m_list;
  Gtk::ToggleButton* m_add;
  Gtk::ScrolledWindow* m_scrolledwindow;
  Gtk::MediaControls* m_mediacontrols;
  Miner* m_miner;
  Glib::RefPtr<Gtk::TreeSelection> m_treeselection;
  Gtk::AspectFrame* m_frame;
  Database_Queries* m_queries;
  Gtk::Label* m_label;
  Decoder m_decoder;
  Glib::RefPtr<Glib::Binding> m_prop_binding;
};
