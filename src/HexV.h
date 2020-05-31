#ifndef HEXVIEWER_H
#define HEXVIEWER_H

#include <cstdint>
#include <glibmm/ustring.h>
#include <gtkmm.h>

class HexV : public Gtk::Window
{
  const size_t width = 475;
  const size_t height = 500;
  const uint32_t textViewWidth = 66;
  char* buffer = nullptr;
  uint32_t bufferSize = 0;

  uint32_t textBufferSize = 0;
  Glib::ustring ustringBuffer;

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
  void about();
  void errorMessage( const Glib::ustring& text, const Glib::ustring& subtext );
  void exit() { hide(); };

public:
  HexV();
};

#endif
