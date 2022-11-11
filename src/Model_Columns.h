/*
 * Copyright © 2022 Diego S.
 *
 */
#pragma once

class Model_Columns : public Gtk::TreeModelColumnRecord {

 public:

  Model_Columns() {
    add(m_col_text);
    add(m_col_path);
  }

  Gtk::TreeModelColumn<Glib::ustring> m_col_text;
  Gtk::TreeModelColumn<Glib::ustring> m_col_path;
};    

Model_Columns m_Columns;

