#ifndef HEXV_H
#define HEXV_H

#include "DataHandler.h"
#include <cstdint>
#include <fstream>
#include <glibmm/ustring.h>
#include <gtkmm.h>
#include <iostream>

class HexV : public Gtk::Window
{
  static const size_t width = 475;
  static const size_t height = 700;

  DataHandler dataHandler;

  struct fileType
  {
    Glib::ustring name, path;
    std::string raw;
  } file;

  // Window
  Gtk::HeaderBar* headerBar;
  Gtk::MenuBar* menubar;
  Gtk::MenuItem* filePlaceHolder;
  Gtk::MenuItem* openPlaceHolder;
  Gtk::MenuItem* closePlaceHolder;
  Gtk::MenuItem* searchPlaceHolder;
  Gtk::MenuItem* textPlaceHolder;
  Gtk::MenuItem* helpPlaceHolder;
  Gtk::MenuItem* aboutPlaceHolder;
  Gtk::Menu* fileMenu;
  Gtk::Menu* searchMenu;
  Gtk::Menu* helpMenu;
  Gtk::Grid* grid;
  Gtk::Box* bigBox;
  Gtk::Box* searchBox;

  Gtk::SearchEntry* searchEntry;
  Glib::RefPtr<Gtk::EntryBuffer> searchBuffer;

  Gtk::ScrolledWindow* scrolledWindow;
  Gtk::TextView* textView;
  Gtk::TextView* hexView;
  Glib::RefPtr<Gtk::CssProvider> cssProvider;
  Glib::RefPtr<Gtk::StyleContext> styleContext;
  Glib::RefPtr<Gtk::TextBuffer> textBuffer;
  Glib::RefPtr<Gtk::TextBuffer> hexBuffer;

  // Targets:
  std::vector<Gtk::TargetEntry> listTargets;

  // Helper
  void parseFilePath( std::string );
  void displaySearchBox();
  void search();
  void disableSearch();
  void openFile();
  void openDroppedFile( const Glib::RefPtr<Gdk::DragContext>& context,
                        int,
                        int,
                        const Gtk::SelectionData& selection_data,
                        guint,
                        guint time );
  void setupWindow();
  void setupMenu();
  void setupPanels();
  void connectEvents();
  void about();
  void displayErrorMessage( const Glib::ustring& text,
                            const Glib::ustring& subtext );
  void exit() { hide(); };

public:
  HexV();
  ~HexV(){};
};

#endif
