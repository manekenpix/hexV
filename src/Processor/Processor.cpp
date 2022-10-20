#include "Processor.h"

void
Processor::loadFile( Glib::ustring fn )
{
  std::ifstream file;
  file.open( fn.c_str(), std::ios::binary );

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
Processor::processFile()
{
  const u32 totalNumberOfPasses = fileSize / PANEL_STRIDE;
  u8 lastElements = fileSize % PANEL_STRIDE;

  textPanel.clear();
  hexPanel.clear();

  textPanel.reserve( fileSize + totalNumberOfPasses );
  hexPanel.reserve( ( HEX_PANEL_STRIDE * totalNumberOfPasses ) +
                    HEX_PANEL_STRIDE );

  auto processChunk = [&]( Glib::ustring& textBuffer,
                           Glib::ustring& hexBuffer,
                           std::vector<char>::iterator fileIterator,
                           u32 passes,
                           u8 stride = PANEL_STRIDE ) {
    for ( u32 pass = 0; pass < passes; ++pass ) {

      for ( u32 i = 0; i < stride; ++i ) {
        textBuffer += *fileIterator >= '!' && *fileIterator <= '~'
                        ? Glib::ustring( 1, *fileIterator )
                        : Glib::ustring( 1, '.' );

        hexBuffer += hexValues[( *fileIterator >> 4 ) & 0x0F];
        hexBuffer += hexValues[( *fileIterator & 0x0F )];
        if ( i != PANEL_STRIDE - 1 )
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
    chunkSize = chunkSize - ( chunkSize % PANEL_STRIDE );
    const u32 numberOfPassesPerChunk = chunkSize / PANEL_STRIDE;

    std::vector<std::thread> pool;
    std::vector<Glib::ustring> textBuffers( numberOfThreads, Glib::ustring() );
    std::vector<Glib::ustring> hexBuffers( numberOfThreads, Glib::ustring() );

    for ( u8 t = 0; t < numberOfThreads; ++t ) {
      textBuffers[t].reserve( chunkSize + numberOfPassesPerChunk );
      hexBuffers[t].reserve( ( HEX_PANEL_STRIDE * numberOfPassesPerChunk ) +
                             HEX_PANEL_STRIDE );
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

    if ( lastElements > PANEL_STRIDE ) {
      u32 lastPasses = lastElements / PANEL_STRIDE;

      std::vector<char>::iterator fileIterator = raw->end() - lastElements;
      processChunk( textPanel, hexPanel, fileIterator, lastPasses );
    }

    lastElements = fileSize % PANEL_STRIDE;

  } else {
    // Single thread
    processChunk( textPanel, hexPanel, raw->begin(), totalNumberOfPasses );
  }

  rawString = std::string( raw->data(), raw->size() );

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

std::vector<char>*
Processor::getRawBuffer() const
{
  return raw;
};

Glib::ustring*
Processor::getTextBuffer()
{
  return &textPanel;
};

Glib::ustring*
Processor::getHexBuffer()
{
  return &hexPanel;
};

std::vector<std::string::size_type>
Processor::searchAll( const Glib::ustring& str )
{
  if ( !str.empty() && !rawString.empty() ) {
    std::string::size_type hit = 0;

    while ( hit != std::string::npos ) {
      hit = rawString.find( str, hit );

      if ( hit != std::string::npos ) {
        searchHits.push_back( hit + ( hit / PANEL_STRIDE ) );
        if ( rawString.length() - hit > str.length() ) {
          hit += str.length();
        } else
          hit = std::string::npos;
      }
    }
  }
  return searchHits;
};

void
Processor::clearSearch()
{
  searchHits.clear();
};

Processor::~Processor()
{
  delete raw;
};
