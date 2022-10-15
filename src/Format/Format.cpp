#include "Format.h"

Format::FileFormats
Format::identifyFormat( std::vector<char>* buffer ) const
{
  for ( const auto& item : formats ) {
    if ( isFormat( item.first, buffer->cbegin() ) )
      return item.first;
  }

  return Format::FileFormats::NF;
};

bool
Format::isFormat( const Format::FileFormats fm,
                  std::vector<char>::const_iterator it ) const
{
  auto formatSignature = formats.at( fm );

  for ( auto byte : formatSignature )
    if ( byte != *it++ )
      return false;

  return true;
};
