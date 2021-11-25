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
  const size_t width = 475;
  const size_t height = 500;

  DataHandler dataHandler;

  struct fileType
  {
    Glib::ustring name, path;
    std::string raw;
  } openedFile;

  // Window
  Gtk::HeaderBar* headerBar;
  Gtk::MenuBar* menubar;
  Gtk::MenuItem* filePlaceHolder;
  Gtk::MenuItem* openPlaceHolder;
  Gtk::MenuItem* closePlaceHolder;
  Gtk::MenuItem* searchPlaceHolder;
  Gtk::MenuItem* textPlaceHolder;
  Gtk::MenuItem* hexPlaceHolder;
  Gtk::MenuItem* helpPlaceHolder;
  Gtk::MenuItem* aboutPlaceHolder;
  Gtk::Menu* fileMenu;
  Gtk::Menu* searchMenu;
  Gtk::Menu* helpMenu;
  Gtk::Box* bigBox;
  Gtk::Box* searchBox;

  Gtk::SearchEntry* searchEntry;
  Glib::RefPtr<Gtk::EntryBuffer> searchBuffer;

  Gtk::ScrolledWindow* scrolledWindow;
  Gtk::TextView* textView;
  Glib::RefPtr<Gtk::CssProvider> cssProvider;
  Glib::RefPtr<Gtk::StyleContext> styleContext;
  Glib::RefPtr<Gtk::TextBuffer> textBuffer;

  // Targets:
  std::vector<Gtk::TargetEntry> listTargets;

  // Helper
  void parseFilePath( std::string );
  void displaySearchBox();
  void searchEvent();
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
