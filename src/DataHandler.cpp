#include "include/DataHandler.h"

void
DataHandler::loadFile( Glib::ustring fn )
{
  std::ifstream file( fn, std::ios::binary );

  if ( file.is_open() ) {
    delete raw;
    delete textPanel;
    delete hexPanel;
    fullPanel.clear();

    fileName = fn;

    raw = new std::vector<char>;
    textPanel = new std::vector<Glib::ustring>;
    hexPanel = new std::vector<HexByte>;

    // Not sure if this is needed
    file.unsetf( std::ios::skipws );

    file.seekg( 0, std::ios::end );
    fileSize = (u32)( file.tellg() );
    file.seekg( 0, std::ios::beg );

    raw->reserve( fileSize );
    raw->insert( raw->begin(),
                 std::istreambuf_iterator<char>( file ),
                 std::istreambuf_iterator<char>() );

    hexPanel->reserve( fileSize );
    textPanel->reserve( fileSize );
    for ( auto& rawIt : *raw ) {
      textPanel->push_back( rawIt > 33 && rawIt < 127
                              ? Glib::ustring( 1, rawIt )
                              : Glib::ustring( 1, '.' ) );

      hexPanel->push_back(
        { hexValues[( rawIt >> 4 ) & 0x0F], hexValues[( rawIt & 0x0F )] } );
    }
  }
};

Glib::ustring*
DataHandler::createView()
{
  const u32 numberOfPasses = ( fileSize / panelStride );
  const u8 lastElements = ( fileSize % panelStride );

  fullPanel.reserve( ( numberOfPasses * stride ) + stride );

  u32 pass;
  for ( pass = 0; pass < numberOfPasses; ++pass ) {

    for ( u32 textIt = 0; textIt < panelStride; ++textIt ) {
      fullPanel += textPanel->at( ( pass * panelStride ) + textIt );
    }

    fullPanel += separator;

    for ( u32 hexIt = 0; hexIt < panelStride; ++hexIt ) {
      fullPanel += ' ';
      fullPanel += hexPanel->at( ( pass * panelStride ) + hexIt ).high;
      fullPanel += hexPanel->at( ( pass * panelStride ) + hexIt ).low;
    }
    fullPanel += newLine;
  }

  // Last pass
  for ( u32 textIt = 0; textIt < lastElements; ++textIt ) {
    fullPanel += textPanel->at( ( pass * panelStride ) + textIt );
  }
  fullPanel.append( panelStride - lastElements, ' ' );
  fullPanel += separator;

  for ( u32 hexIt = 0; hexIt < lastElements; ++hexIt ) {
    fullPanel += ' ';
    fullPanel += hexPanel->at( ( pass * panelStride ) + hexIt ).high;
    fullPanel += hexPanel->at( ( pass * panelStride ) + hexIt ).low;
  }

  return &fullPanel;
};

std::string::size_type
DataHandler::findInstances( const Glib::ustring search ) const
{
  if ( !textPanel || search.length() == 0 )
    return std::string::npos;

  return fullPanel.find( search );
};

DataHandler::~DataHandler()
{
  delete raw;
  delete textPanel;
  delete hexPanel;
};
