/*
 * Copyright © 2022 Diego S.
 *
 */
#pragma once

class Model_Columns : public Gtk::TreeModelColumnRecord {

 public:

  Model_Columns() {
    add(m_col_text);
    add(m_col_number);
  }

  Gtk::TreeModelColumn<Glib::ustring> m_col_text;
  Gtk::TreeModelColumn<int> m_col_number;  
};    

Model_Columns m_Columns;

