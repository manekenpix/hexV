#include "include/HexV.h"

HexV::HexV()
{
  setupWindow();
  
  std::fstream iconFile;
  iconFile.open( "images/hv.png", std::fstream::in );
  
  if ( iconFile.is_open() ) {
    iconFile.close();
    set_icon_from_file( "images/hv.png" );
  }
  
  setupMenu();
  
  setupPanels();
  
  // Search Entry
  searchEntry = Gtk::manage( new Gtk::SearchEntry() );
  searchBuffer = Gtk::EntryBuffer::create();
  searchEntry->set_buffer( searchBuffer );
  
  // Boxes
  searchBox = Gtk::manage( new Gtk::Box( Gtk::ORIENTATION_VERTICAL, 0 ) );
  searchBox->pack_start( *searchEntry, true, true, 0 );
  
  bigBox = Gtk::manage( new Gtk::Box( Gtk::ORIENTATION_VERTICAL, 0 ) );
  bigBox->pack_start( *menubar, Gtk::PACK_SHRINK, 0 );
  bigBox->pack_start( *searchBox, false, false, 0 );
  bigBox->pack_start( *scrolledWindow, true, true, 0 );
  
  // Events
  connectEvents();
  
  add( *bigBox );
  show_all();
  
  disableSearch();
};

void
HexV::setupWindow()
{
  set_default_size( width, height );
  set_resizable( false );
  set_position( Gtk::WIN_POS_CENTER_ALWAYS );
  set_resizable( false );
};

void
HexV::setupMenu()
{
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
  searchPlaceHolder = Gtk::manage( new Gtk::MenuItem( "_Search", true ) );
  textPlaceHolder = Gtk::manage( new Gtk::MenuItem( "_Text", true ) );
  helpPlaceHolder = Gtk::manage( new Gtk::MenuItem( "_Help", true ) );
  aboutPlaceHolder = Gtk::manage( new Gtk::MenuItem( "_About", true ) );
  
  fileMenu = Gtk::manage( new Gtk::Menu() );
  searchMenu = Gtk::manage( new Gtk::Menu() );
  helpMenu = Gtk::manage( new Gtk::Menu() );
  
  menubar->append( *filePlaceHolder );
  filePlaceHolder->set_submenu( *fileMenu );
  fileMenu->append( *openPlaceHolder );
  fileMenu->append( *closePlaceHolder );
  
  menubar->append( *searchPlaceHolder );
  searchPlaceHolder->set_submenu( *searchMenu );
  searchMenu->append( *textPlaceHolder );
  
  menubar->append( *helpPlaceHolder );
  helpPlaceHolder->set_submenu( *helpMenu );
  helpMenu->append( *aboutPlaceHolder );
};

void
HexV::setupPanels()
{
  listTargets.push_back( Gtk::TargetEntry( "text/uri-list" ) );
  
  textView = Gtk::manage( new Gtk::TextView() );
  textView->set_wrap_mode( Gtk::WRAP_WORD );
  textView->drag_dest_set( listTargets );
  
  textBuffer = Gtk::TextBuffer::create();
  textView->set_buffer( textBuffer );
  
  // Hex Viewer
  hexView = Gtk::manage( new Gtk::TextView() );
  hexView->set_wrap_mode( Gtk::WRAP_WORD );
  hexView->drag_dest_set( listTargets );
  
  hexBuffer = Gtk::TextBuffer::create();
  hexView->set_buffer( hexBuffer );
  
  Glib::ustring css = ".view {font-family: monospace; font-size: 11px;}";
  cssProvider = Gtk::CssProvider::create();
  
  try {
    cssProvider->load_from_data( css );
    textView->get_style_context()->add_provider_for_screen(
                                                           Gdk::Screen::get_default(),
                                                           cssProvider,
                                                           GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );
    
    hexView->get_style_context()->add_provider_for_screen(
                                                          Gdk::Screen::get_default(),
                                                          cssProvider,
                                                          GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );
  } catch ( const Gtk::CssProviderError& ex ) {
    std::cerr << "CssProviderError: " << ex.what() << std::endl;
  }
  
  grid = Gtk::manage( new Gtk::Grid() );
  grid->set_column_homogeneous( true );
  grid->set_row_spacing( 0 );
  grid->set_column_spacing( 0 );
  grid->attach( *textView, 0, 0, 1, 1 );
  grid->attach( *hexView, 1, 0, 3, 1 );
  
  scrolledWindow = Gtk::manage( new Gtk::ScrolledWindow() );
  scrolledWindow->set_policy( Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS );
  scrolledWindow->set_border_width( 5 );
  scrolledWindow->add( *grid );
};

void
HexV::connectEvents()
{
  // Menu Bar
  openPlaceHolder->signal_activate().connect(
                                             sigc::mem_fun( *this, &HexV::openFile ) );
  closePlaceHolder->signal_activate().connect(
                                              sigc::mem_fun( *this, &HexV::exit ) );
  textPlaceHolder->signal_activate().connect(
                                             sigc::mem_fun( *this, &HexV::displaySearchBox ) );
  aboutPlaceHolder->signal_activate().connect(
                                              sigc::mem_fun( *this, &HexV::about ) );
  textView->signal_drag_data_received().connect(
                                                sigc::mem_fun( *this, &HexV::openDroppedFile ) );
  
  // Search Entry
  searchEntry->signal_search_changed().connect(
                                               sigc::mem_fun( *this, &HexV::search ) );
  searchEntry->signal_stop_search().connect(
                                            sigc::mem_fun( *this, &HexV::disableSearch ) );
};

void
HexV::openFile()
{
  Gtk::FileChooserDialog fileDialog(
                                    *this, Glib::ustring( "Open File" ), Gtk::FILE_CHOOSER_ACTION_OPEN );
  fileDialog.add_button( Glib::ustring( "_Open" ), Gtk::RESPONSE_OK );
  fileDialog.add_button( Glib::ustring( "_Cancel" ), Gtk::RESPONSE_CANCEL );
  
  int response = fileDialog.run();
  fileDialog.close();
  
  if ( response == Gtk::RESPONSE_OK ) {
    if ( Glib::ustring( fileDialog.get_filename() ) != file.path ) {
      
      parseFilePath( Glib::ustring( fileDialog.get_filename() ) );
      headerBar->set_subtitle( file.name );
      
      dataHandler.loadFile( file.path );
      textBuffer->set_text( *( dataHandler.getTextBuffer() ) );
      hexBuffer->set_text( *( dataHandler.getHexBuffer() ) );
      
    } else
      displayErrorMessage( "Error Opening File", "File already open" );
  }
};

void
HexV::openDroppedFile( const Glib::RefPtr<Gdk::DragContext>& context,
                      int,
                      int,
                      const Gtk::SelectionData& selection_data,
                      guint,
                      guint time )
{
  context->drag_finish( false, false, time );
  
  if ( ( selection_data.get_length() >= 0 ) &&
      ( selection_data.get_format() == 8 ) ) {
    
    if ( selection_data.get_data_as_string() != file.raw ) {
      parseFilePath( selection_data.get_data_as_string() );
      headerBar->set_subtitle( file.name );
      
      dataHandler.loadFile( file.path );
      textBuffer->set_text( *( dataHandler.getTextBuffer() ) );
      hexBuffer->set_text( *( dataHandler.getHexBuffer() ) );
    }
  } else
    displayErrorMessage( "Error Opening File",
                        "There was a problem accessing the file" );
};

void
HexV::displaySearchBox()
{
  searchBox->show();
  searchEntry->grab_focus();
};

void
HexV::search()
{
  const Glib::ustring searchedText = searchBuffer->get_text();
  std::string::size_type position = dataHandler.search( searchedText );
  
  if ( position != std::string::npos ) {
    Gtk::TextIter temp =
      textBuffer->get_iter_at_offset( static_cast<int>( position ) );
    Gtk::TextIter it = temp, anotherIt = temp;
    
    for ( u32 i = 0; i < searchedText.length(); ++i )
      ++anotherIt;
    
    textBuffer->select_range( static_cast<const Gtk::TextIter>( it ),
                             static_cast<const Gtk::TextIter>( anotherIt ) );
  } else {
    textBuffer->select_range( textBuffer->begin(), textBuffer->begin() );
  }
};

void
HexV::disableSearch()
{
  searchBox->hide();
  
  if ( textBuffer ) {
    textBuffer->select_range(
                             static_cast<const Gtk::TextIter>( textBuffer->begin() ),
                             static_cast<const Gtk::TextIter>( textBuffer->begin() ) );
  }
};

void
HexV::about()
{
  Gtk::AboutDialog aboutDialog;
  aboutDialog.set_authors( { "manekenpix" } );
  aboutDialog.set_website( "https://github.com/manekenpix" );
  aboutDialog.set_icon_from_file( "images/hv.png" );
  aboutDialog.set_program_name( "hexV" );
  aboutDialog.set_version( "0.1" );
  aboutDialog.set_license_type( Gtk::LICENSE_MIT_X11 );
  aboutDialog.set_comments(
                           "Small viewer that shows the content of a file in hexadecimal" );
  
  Glib::RefPtr<Gdk::Pixbuf> logo =
    Gdk::Pixbuf::create_from_file( "images/hv.png", 100, 100, true );
  aboutDialog.set_logo( logo );
  aboutDialog.run();
};

void
HexV::displayErrorMessage( const Glib::ustring& text,
                          const Glib::ustring& subtext )
{
  Gtk::MessageDialog dialog( *this, text );
  dialog.set_secondary_text( subtext );
  
  dialog.run();
};

void
HexV::parseFilePath( std::string bStr )
{
  const Glib::ustring encodedSpace( "%20" );
  const Glib::ustring localPath( "file://" );
  
  file.raw = bStr;
  
  if ( bStr.find( encodedSpace ) != std::string::npos )
    bStr =
    bStr.replace( bStr.find( encodedSpace ), encodedSpace.length(), " " );
  
  if ( bStr.find( localPath ) != std::string::npos )
    bStr =
    bStr.substr( localPath.length(), bStr.length() - localPath.length() - 2 );
  
  file.path = Glib::ustring( bStr );
  
  Glib::ustring::size_type lastSlash = file.path.find_last_of( "/" );
  file.name =
    file.path.substr( lastSlash + 1, file.path.size() - lastSlash );
};
