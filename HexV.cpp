#include <fstream>
#include <iostream>
#include "HexV.h"

HexV::HexV()
{
  // Window
  set_default_size( width, height );
  set_resizable( false );
  set_position( Gtk::WIN_POS_CENTER_ALWAYS );
  set_resizable( false );

  std::fstream iconFile;
  iconFile.open( "hv.png", std::fstream::in );
  if ( iconFile.is_open() ) {
    iconFile.close();
    set_icon_from_file( "hv.png" );
  }

  // Header bar
  headerBar = Gtk::manage( new Gtk::HeaderBar() );
  headerBar->set_has_subtitle( true );
  headerBar->set_title( "hexV" );
  headerBar->set_show_close_button( true );
  set_titlebar( *headerBar );

  // Menu bar
  menubar = Gtk::manage( new Gtk::MenuBar() );

  filePlaceHolder = Gtk::manage( new Gtk::MenuItem( "_File", true ) );
  openPlaceHolder = Gtk::manage( new Gtk::MenuItem( "_Open", true ) );
  closePlaceHolder = Gtk::manage( new Gtk::MenuItem( "_Close", true ) );
  helpPlaceHolder = Gtk::manage( new Gtk::MenuItem( "_Help", true ) );
  aboutPlaceHolder = Gtk::manage( new Gtk::MenuItem( "_About", true ) );

  fileMenu = Gtk::manage( new Gtk::Menu() );
  helpMenu = Gtk::manage( new Gtk::Menu() );

  // Boxes
  bigBox = Gtk::manage( new Gtk::Box( Gtk::ORIENTATION_VERTICAL, 0 ) );

  // Menubar
  menubar->append( *filePlaceHolder );
  filePlaceHolder->set_submenu( *fileMenu );
  fileMenu->append( *openPlaceHolder );
  fileMenu->append( *closePlaceHolder );

  menubar->append( *helpPlaceHolder );
  helpPlaceHolder->set_submenu( *helpMenu );
  helpMenu->append( *aboutPlaceHolder );

  // Events
  openPlaceHolder->signal_activate().connect( sigc::mem_fun( *this, &HexV::openFile ) );
  closePlaceHolder->signal_activate().connect( sigc::mem_fun( *this, &HexV::exit ) );
  aboutPlaceHolder->signal_activate().connect( sigc::mem_fun( *this, &HexV::about ) );

  // Text Viewer
  textView = Gtk::manage( new Gtk::TextView() );
  textView->set_wrap_mode( Gtk::WRAP_WORD );

  textBuffer = Gtk::TextBuffer::create();

  textView->set_buffer( textBuffer );
  textView->set_monospace();

  // Scrolled Window
  scrolledWindow = Gtk::manage( new Gtk::ScrolledWindow() );
  scrolledWindow->set_policy( Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS );
  scrolledWindow->set_border_width( 5 );
  scrolledWindow->add( *textView );

  // Position Box
  position = Gtk::manage( new Gtk::Label() );
  position->set_label( Glib::ustring( topText ) );
  position->set_halign( Gtk::Align::ALIGN_START );
  auto context = position->get_pango_context();
  auto fontDescription = context->get_font_description();
  fontDescription.set_family( "Monospace" );
  context->set_font_description( fontDescription );

  bigBox->pack_start( *menubar, Gtk::PACK_SHRINK, 0 );
  // bigBox->pack_start( *position, false, false, 10 );
  bigBox->pack_start( *scrolledWindow, true, true, 0 );

  add( *bigBox );
  show_all();
}

void HexV::openFile()
{
  Gtk::FileChooserDialog fileDialog( *this, Glib::ustring( "Open File" ), Gtk::FILE_CHOOSER_ACTION_OPEN );
  fileDialog.add_button( Glib::ustring( "_Open" ), Gtk::RESPONSE_OK );
  fileDialog.add_button( Glib::ustring( "_Cancel" ), Gtk::RESPONSE_CANCEL );

  int response = fileDialog.run();
  fileDialog.close();

  if ( response == Gtk::RESPONSE_OK ) {
    std::cout << "Opening: " << fileDialog.get_filename() << std::endl;
    std::fstream file;
    file.open( fileDialog.get_filename(), std::fstream::in | std::fstream::binary );

    if ( file.is_open() ) {
      Glib::ustring fileName( fileDialog.get_filename() );
      Glib::ustring::size_type lastSlash = fileName.find_last_of( "/" );
      Glib::ustring::size_type size = fileName.size();

      fileName = fileName.substr( lastSlash + 1, size - lastSlash );
      headerBar->set_subtitle( fileName );

      file.seekg( 0, std::ios::end );
      bufferSize = ( uint32_t )( file.tellg() );
      file.seekg( 0, std::ios::beg );

      if ( bufferSize ) {
        buffer = new char[bufferSize];
        file.read( ( buffer ), bufferSize );
        process();
        delete[] buffer;
      }
      file.close();
    }
    else
      errorMessage( "Error Opening File", "There was a problem accessing the file" );
  }
}

void HexV::process()
{
  char high, low;
  uint32_t textViewBufferIndex = 3; // first 19 characters
  Glib::ustring ascii, hex, line;
  ustringBuffer = "";

  for ( uint32_t i = 0; i < bufferSize; ++i ) {
    high = byteToChar( ( buffer[i] >> 4 ) & 0x0F );
    low = byteToChar( buffer[i] & 0x0F );

    ascii += buffer[i] > 33 && buffer[i] < 127 ? Glib::ustring( 1, buffer[i] ) : Glib::ustring( 1, '.' );
    textViewBufferIndex += 1;

    hex += Glib::ustring( 1, high );
    hex += Glib::ustring( 1, low );
    textViewBufferIndex += 2;

    if ( textViewBufferIndex % textViewWidth == 0 ) {
      line = ascii + " | " + hex + Glib::ustring( 1, '\n' );
      ustringBuffer += line;
      line = hex = ascii = "";
      textViewBufferIndex += 3;
    }
    else {
      hex += Glib::ustring( 1, ' ' );
      textViewBufferIndex += 1;
    }
  }
  textBuffer->set_text( ustringBuffer );

  std::cout << "Size of bufferSize: " << bufferSize << std::endl;
  std::cout << "textViewBufferIndex: " << textViewBufferIndex << std::endl;
}

void HexV::about()
{
  Gtk::AboutDialog aboutD;
  aboutD.set_authors( {"Josue Quilon Barrios"} );
  aboutD.set_website( "https://github.com/manekenpix" );
  aboutD.set_icon_from_file( "hv.png" );
  aboutD.set_program_name( "hexV" );
  aboutD.set_version( "0.0.1" );
  aboutD.set_license_type( Gtk::LICENSE_MIT_X11 );
  aboutD.set_comments( "Small viewer that shows the content of a file in hexadecimal" );

  Glib::RefPtr<Gdk::Pixbuf> logo = Gdk::Pixbuf::create_from_file( "hv.png", 100, 100, true );
  aboutD.set_logo( logo );
  aboutD.run();
}

void HexV::errorMessage( Glib::ustring text, Glib::ustring subtext )
{
  Gtk::MessageDialog dialog( *this, text );
  dialog.set_secondary_text( subtext );

  dialog.run();
}
