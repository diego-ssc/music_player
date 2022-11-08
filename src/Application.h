/*
 * Copyright © 2022 Diego S.
 *
 */

#pragma once

#include <gtkmm.h>

class Application_Window;

class Application: public Gtk::Application {

 protected:
  Application();

 public:
  static Glib::RefPtr<Application> create();

 protected:
  void on_activate() override;
  void on_open(const Gio::Application::type_vec_files& files,
               const Glib::ustring& hint) override;

 private:
  Application_Window* create_appwindow();
  void on_hide_window(Gtk::Window* window);


};
