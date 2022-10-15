#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "../types.h"
#include <array>
#include <chrono>
#include <fstream>
#include <glibmm-2.4/glibmm.h>
#include <thread>
#include <vector>

class Processor
{
  static const u8 panelStride = 16;
  static const u8 hexPanelStride = 48;

  const Glib::ustring newLine = "\n";

  const u32 bytesToProcessPerThread = 2000000;
  std::vector<char>* raw = nullptr;
  Glib::ustring fileName, textPanel, hexPanel;

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
  std::string::size_type search(
    const Glib::ustring& str,
    const std::string::size_type position = 0 ) const;
};

#endif
