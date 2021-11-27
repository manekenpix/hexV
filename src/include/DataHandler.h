#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include "types.h"
#include <array>
#include <fstream>
#include <glibmm/ustring.h>
#include <vector>

class DataHandler
{
  static const u8 panelStride = 16;
  static const u8 hexPanelStride = 48;

  const Glib::ustring newLine = "\n";

  std::vector<char>* raw = nullptr;
  Glib::ustring fileName, textPanel, hexPanel;

  u32 fileSize = 0;

  const std::array<Glib::ustring, 16> hexValues = { "0", "1", "2", "3",
                                                    "4", "5", "6", "7",
                                                    "8", "9", "A", "B",
                                                    "C", "D", "E", "F" };

public:
  DataHandler(){};
  ~DataHandler();

  void loadFile( Glib::ustring );
  void createView();
  Glib::ustring* getTextBuffer();
  Glib::ustring* getHexBuffer();
  std::string::size_type search( const Glib::ustring str ) const;
};

#endif
