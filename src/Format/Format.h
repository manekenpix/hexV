#ifndef FORMATS_H
#define FORMATS_H

#include "../types.h"
#include <fstream>
#include <unordered_map>
#include <vector>

class Format
{
public:
  enum class FileFormats
  {
    PNG,
    JPEG,
    BMP,
    NF,
  };

  Format(){};
  ~Format(){};

  FileFormats identifyFormat( std::vector<char>* buffer ) const;

private:
  std::unordered_map<typename Format::FileFormats, typename std::vector<char>>
    formats = { { Format::FileFormats::BMP, std::vector<char>( { 77, 66 } ) },
                { Format::FileFormats::JPEG,
                  std::vector<char>( { -1, -40, -1 } ) },
                { Format::FileFormats::PNG,
                  std::vector<char>( { -119, 80, 78, 71, 13, 10, 26, 10 } ) } };

  bool isFormat( const Format::FileFormats fm,
                 std::vector<char>::const_iterator it ) const;
};

#endif
