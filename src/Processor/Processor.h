#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "../types.h"
#include <array>
#include <fstream>
#include <glibmm-2.4/glibmm.h>
#include <thread>
#include <vector>

class Processor
{
  static const u8 PANEL_STRIDE = 16;
  static const u8 HEX_PANEL_STRIDE = 48;

  const Glib::ustring newLine = "\n";

  const u32 bytesToProcessPerThread = 2000000;
  std::vector<char>* raw = nullptr;
  Glib::ustring fileName, textPanel, hexPanel;
  std::string rawString;

  // Search
  const u8 SEARCH_LIMIT = 200;
  std::vector<std::string::size_type> searchHits;

  u32 fileSize = 0;

  const std::array<Glib::ustring, 16> hexValues = { "0", "1", "2", "3",
                                                    "4", "5", "6", "7",
                                                    "8", "9", "A", "B",
                                                    "C", "D", "E", "F" };

  void processFile();

public:
  Processor(){};
  ~Processor();

  void loadFile( Glib::ustring );
  std::vector<char>* getRawBuffer() const;
  Glib::ustring* getTextBuffer();
  Glib::ustring* getHexBuffer();
  std::vector<std::string::size_type> searchAll( const Glib::ustring& str );
  void clearSearch();
};

#endif
