#include "include/DataHandler.h"

void
DataHandler::loadFile( Glib::ustring fn )
{
  std::ifstream file( fn, std::ios::binary );

  if ( file.is_open() ) {
    delete raw;
    fileName = fn;

    raw = new std::vector<char>;
    file.unsetf( std::ios::skipws );

    file.seekg( 0, std::ios::end );
    fileSize = (u32)( file.tellg() );
    file.seekg( 0, std::ios::beg );

    raw->reserve( fileSize );
    raw->insert( raw->begin(),
                 std::istreambuf_iterator<char>( file ),
                 std::istreambuf_iterator<char>() );

    createView();
  }
};

void
DataHandler::createView()
{
  const u32 numberOfPasses = ( fileSize / panelStride );
  const u8 lastElements = ( fileSize % panelStride );

  textPanel.clear();
  hexPanel.clear();

  textPanel.reserve( fileSize + numberOfPasses );
  hexPanel.reserve( ( hexPanelStride * numberOfPasses ) + hexPanelStride );

  u32 pass, line;
  for ( pass = 0; pass < numberOfPasses; ++pass ) {

    line = pass * panelStride;
    for ( u32 textIt = 0; textIt < panelStride; ++textIt ) {

      textPanel +=
        raw->at( line + textIt ) > 33 && raw->at( line + textIt ) < 127
          ? Glib::ustring( 1, raw->at( line + textIt ) )
          : Glib::ustring( 1, '.' );

      hexPanel += hexValues[( raw->at( line + textIt ) >> 4 ) & 0x0F];
      hexPanel += hexValues[( raw->at( line + textIt ) & 0x0F )];
      if ( textIt != panelStride - 1 )
        hexPanel += ' ';
    }

    textPanel += newLine;
    hexPanel += newLine;
  }

  // Last pass
  line += panelStride;
  for ( u32 textIt = 0; textIt < lastElements; ++textIt ) {

    textPanel += raw->at( line + textIt ) > 33 && raw->at( line + textIt ) < 127
                   ? Glib::ustring( 1, raw->at( line + textIt ) )
                   : Glib::ustring( 1, '.' );

    hexPanel += hexValues[( raw->at( line + textIt ) >> 4 ) & 0x0F];
    hexPanel += hexValues[( raw->at( line + textIt ) & 0x0F )];
    hexPanel += ' ';
  }
};

Glib::ustring*
DataHandler::getTextBuffer()
{
  return &textPanel;
};

Glib::ustring*
DataHandler::getHexBuffer()
{
  return &hexPanel;
};

std::string::size_type
DataHandler::search( const Glib::ustring search ) const
{
  if ( textPanel.length() == 0 || search.length() == 0 )
    return std::string::npos;

  return textPanel.find( search );
};

DataHandler::~DataHandler()
{
  delete raw;
};
