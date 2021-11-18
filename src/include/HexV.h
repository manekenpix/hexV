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
  
  int fileChooserResponse;
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
  Gtk::MenuItem* helpPlaceHolder;
  Gtk::MenuItem* aboutPlaceHolder;
  Gtk::Menu* fileMenu;
  Gtk::Menu* helpMenu;
  Gtk::Box* bigBox;
  
  Gtk::Label* position;
  Gtk::ScrolledWindow* scrolledWindow;
  Gtk::TextView* textView;
  Glib::RefPtr<Gtk::CssProvider> cssProvider;
  Glib::RefPtr<Gtk::StyleContext> styleContext;
  Glib::RefPtr<Gtk::TextBuffer> textBuffer;
  
  // Targets:
  std::vector<Gtk::TargetEntry> listTargets;
  
  // Helper
  void parseFilePath( std::string );
  void openFile();
  void openDroppedFile( const Glib::RefPtr<Gdk::DragContext>& context,
                       int,
                       int,
                       const Gtk::SelectionData& selection_data,
                       guint,
                       guint time );
  void process();
  inline uint8_t byteToChar( char c )
  {
    return ( c < 10 ? ( c += 48 ) : ( c += 55 ) );
  };
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
