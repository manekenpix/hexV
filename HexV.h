#ifndef HEXVIEWER_H
#define HEXVIEWER_H

#include <cstdint>
#include <gtkmm.h>
#include <glibmm/ustring.h>

class HexV : public Gtk::Window
{
  const size_t width = 400;
  const size_t height = 500;
  const uint32_t textViewWidth = 55;
  const char * topText = " Position                  00 01 02 03 04 05 06 07 08 09";

  uint32_t textBufferSize = 0;
  Glib::ustring ustringBuffer;

  /****** Window ******/
  Gtk::HeaderBar * headerBar;
  Gtk::MenuBar * menubar;
  Gtk::MenuItem * filePlaceHolder;
  Gtk::MenuItem * openPlaceHolder;
  Gtk::MenuItem * closePlaceHolder;
  Gtk::MenuItem * helpPlaceHolder;
  Gtk::MenuItem * aboutPlaceHolder;
  Gtk::Menu * fileMenu;
  Gtk::Menu * helpMenu;
  Gtk::Box * bigBox;

  Gtk::Label * position;
  Gtk::ScrolledWindow * scrolledWindow;
  Gtk::TextView * textView;
  Glib::RefPtr<Gtk::TextBuffer> textBuffer;

  void openFile();
  inline uint8_t byteToChar( char c )
  {
    return ( c < 10 ? ( c += 48 ) : ( c += 55 ) );
  };
  void about();
  uint32_t textBufferSizeFromBufferSize( uint32_t bufferS );
  void errorMessage( Glib::ustring text, Glib::ustring subtext );
  void exit()
  {
    hide();
  };

  public:
  HexV();
};

#endif
