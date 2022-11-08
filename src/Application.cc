/*
 * Copyright © 2022 Diego S.
 *
 */
#include "Application.h"
#include "Application_Window.h"
#include "Miner.h"
#include <iostream>
#include <exception>

Application::Application()
  : Gtk::Application("proyecto2.musicplayer",
                     Gio::Application::Flags::HANDLES_OPEN) {}

Glib::RefPtr<Application> Application::create() {
  return Glib::make_refptr_for_instance<Application>(new Application());
}

Application_Window* Application::create_appwindow() {
  auto appwindow = Application_Window::create();
  add_window(*appwindow);
  appwindow->signal_hide().connect
    (sigc::bind(sigc::mem_fun(*this, &Application::on_hide_window), appwindow));

  return appwindow;
}

void Application::on_activate() {
  try {
  auto *appwindow = create_appwindow();
  appwindow->present();
  } catch (const Glib::Error& ex) {
    std::cerr << "Application::on_activate(): " << ex.what() << std::endl;
  } catch (const std::exception& ex) {
    std::cerr << "Application::on_activate(): " << ex.what() << std::endl;
  }
}

void Application::on_open(const Gio::Application::type_vec_files& files,
                          const Glib::ustring&) {
  Application_Window* appwindow = nullptr;
  auto windows = get_windows();
  if (windows.size() > 0)
    appwindow = dynamic_cast<Application_Window*>(windows[0]);

  try {
    if (!appwindow)
      appwindow = create_appwindow();

    Miner miner = Miner(files[0]->get_path());
    miner.recursive_search();

    appwindow->present();
  } catch (const Glib::Error& ex) {
    std::cerr << "Application::on_open(): " << ex.what() << std::endl;
  } catch (const std::exception& ex) {
    std::cerr << "Application::on_open(): " << ex.what() << std::endl;
  }
}

void Application::on_hide_window(Gtk::Window* window) {
  delete window;
}
