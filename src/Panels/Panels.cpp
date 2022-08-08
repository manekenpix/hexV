#include "Panels.h"

void
Panels::loadFile( Glib::ustring fn )
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

    processFile();
  }
};

void
Panels::processFile()
{
  const u32 totalNumberOfPasses = fileSize / panelStride;
  u8 lastElements = fileSize % panelStride;

  textPanel.clear();
  hexPanel.clear();

  textPanel.reserve( fileSize + totalNumberOfPasses );
  hexPanel.reserve( ( hexPanelStride * totalNumberOfPasses ) + hexPanelStride );

  auto processChunk = [&]( Glib::ustring& textBuffer,
                           Glib::ustring& hexBuffer,
                           std::vector<char>::iterator fileIterator,
                           u32 passes,
                           u8 stride = panelStride ) {
    for ( u32 pass = 0; pass < passes; ++pass ) {

      for ( u32 i = 0; i < stride; ++i ) {
        textBuffer += *fileIterator >= '!' && *fileIterator <= '~'
                        ? Glib::ustring( 1, *fileIterator )
                        : Glib::ustring( 1, '.' );

        hexBuffer += hexValues[( *fileIterator >> 4 ) & 0x0F];
        hexBuffer += hexValues[( *fileIterator & 0x0F )];
        if ( i != panelStride - 1 )
          hexBuffer += ' ';

        ++fileIterator;
      }

      textBuffer += newLine;
      hexBuffer += newLine;
    }
  };

  // Multi thread
  if ( fileSize > bytesToProcessPerThread ) {

    const u8 numberOfThreads = std::thread::hardware_concurrency() + 1;

    u32 chunkSize = fileSize / numberOfThreads;
    chunkSize = chunkSize - ( chunkSize % panelStride );
    const u32 numberOfPassesPerChunk = chunkSize / panelStride;

    std::vector<std::thread> pool;
    std::vector<Glib::ustring> textBuffers( numberOfThreads, Glib::ustring() );
    std::vector<Glib::ustring> hexBuffers( numberOfThreads, Glib::ustring() );

    for ( u8 t = 0; t < numberOfThreads; ++t ) {
      textBuffers[t].reserve( chunkSize + numberOfPassesPerChunk );
      hexBuffers[t].reserve( ( hexPanelStride * numberOfPassesPerChunk ) +
                             hexPanelStride );
      pool.push_back( std::thread( processChunk,
                                   std::ref( textBuffers[t] ),
                                   std::ref( hexBuffers[t] ),
                                   raw->begin() + ( chunkSize * t ),
                                   numberOfPassesPerChunk ) );
    }

    for ( auto& it : pool )
      it.join();

    for ( auto itT = textBuffers.cbegin(), itH = hexBuffers.cbegin();
          itT < textBuffers.end();
          ++itT, ++itH ) {
      textPanel += *itT;
      hexPanel += *itH;
    }

    // Last chunk
    lastElements = fileSize - ( chunkSize * numberOfThreads );

    if ( lastElements > panelStride ) {
      u32 lastPasses = lastElements / panelStride;

      std::vector<char>::iterator fileIterator = raw->end() - lastElements;
      processChunk( textPanel, hexPanel, fileIterator, lastPasses );
    }

    lastElements = fileSize % panelStride;

  } else {
    // Single thread
    processChunk( textPanel, hexPanel, raw->begin(), totalNumberOfPasses );
  }

  // Last pass
  std::vector<char>::iterator fileIterator = raw->end() - lastElements;
  for ( u32 i = 0; i < lastElements; ++i ) {
    textPanel += *fileIterator >= '!' && *fileIterator <= '~'
                   ? Glib::ustring( 1, *fileIterator )
                   : Glib::ustring( 1, '.' );

    hexPanel += hexValues[( *fileIterator >> 4 ) & 0x0F];
    hexPanel += hexValues[( *fileIterator & 0x0F )];
    hexPanel += ' ';

    ++fileIterator;
  }
};

Glib::ustring*
Panels::getTextBuffer()
{
  return &textPanel;
};

Glib::ustring*
Panels::getHexBuffer()
{
  return &hexPanel;
};

std::string::size_type
Panels::search( const Glib::ustring& str,
                const std::string::size_type pos ) const
{
  return ( textPanel.length() == 0 || str.length() == 0 )
           ? std::string::npos
           : textPanel.find( str, pos );
};

Panels::~Panels()
{
  delete raw;
};
