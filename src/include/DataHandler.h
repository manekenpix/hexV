#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include "types.h"
#include <algorithm>
#include <array>
#include <fstream>
#include <glibmm/ustring.h>
#include <vector>

class DataHandler
{
  const u8 panelStride = 16;
  const u8 spaces = 16;

  const Glib::ustring separator = " |";
  const Glib::ustring newLine = "\n";

  const u8 hexPanelStride =
    ( panelStride * 2 ) + spaces + separator.size() + newLine.size();
  const u8 stride = panelStride + hexPanelStride;

  struct HexByte
  {
    Glib::ustring high;
    Glib::ustring low;
  };

  std::vector<Glib::ustring>* textPanel = nullptr;
  std::vector<HexByte>* hexPanel = nullptr;
  std::vector<char>* raw = nullptr;
  Glib::ustring fullPanel, fileName;

  u32 fileSize = 0;

  const std::array<Glib::ustring, 16> hexValues = { "0", "1", "2", "3",
                                                    "4", "5", "6", "7",
                                                    "8", "9", "A", "B",
                                                    "C", "D", "E", "F" };

public:
  DataHandler(){};
  ~DataHandler();

  void loadFile( Glib::ustring );
  Glib::ustring* createView();
};

#endif
