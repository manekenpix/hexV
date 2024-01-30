#include "HexV.h"

HexV::HexV()
{
  setupWindow();
  setupMenu();
  setupPanels();
  setupSearchBox();
  setupEvents();
  setupContainers();
  disableSearch();
};

void
HexV::setupWindow()
{
  set_default_size( WIDTH, HEIGHT );
  set_position( Gtk::WIN_POS_CENTER_ALWAYS );
  set_resizable( false );

  std::fstream iconFile;
  iconFile.open( ICON_FILE_NAME, std::fstream::in );

  if ( iconFile.is_open() ) {
    iconFile.close();
    set_icon_from_file( ICON_FILE_NAME );
  }
};

void
HexV::setupSearchBox()
{
  searchEntry = Gtk::manage( new Gtk::SearchEntry() );
  searchBuffer = Gtk::EntryBuffer::create();
  searchEntry->set_buffer( searchBuffer );
  searchLabel = Gtk::manage( new Gtk::Label( "0 / 0" ) );

  next = Gtk::manage( new Gtk::Button() );
  previous = Gtk::manage( new Gtk::Button() );

  nextIcon = Gtk::manage( new Gtk::Image(
    Gdk::Pixbuf::create_from_file( "images/arrow-down.svg", 24, 24, true ) ) );
  next->set_image( *nextIcon );

  previousIcon = Gtk::manage( new Gtk::Image(
    Gdk::Pixbuf::create_from_file( "images/arrow-up.svg", 24, 24, true ) ) );
  previous->set_image( *previousIcon );

  searchBox = Gtk::manage( new Gtk::Box( Gtk::ORIENTATION_HORIZONTAL, 0 ) );
  searchBox->pack_start( *searchEntry, true, true, 5 );
  searchBox->pack_start( *searchLabel, false, false, 5 );
  searchBox->pack_start( *next, false, false, 5 );
  searchBox->pack_start( *previous, false, false, 5 );
};

void
HexV::setupContainers()
{
  bigBox = Gtk::manage( new Gtk::Box( Gtk::ORIENTATION_VERTICAL, 0 ) );
  bigBox->pack_start( *menubar, Gtk::PACK_SHRINK, 0 );
  bigBox->pack_start( *searchBox, false, false, 0 );
  bigBox->pack_start( *scrolledWindow, true, true, 0 );

  add( *bigBox );
  show_all();
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
  optionsPlaceHolder = Gtk::manage( new Gtk::MenuItem( "_Options", true ) );
  nextSearchPlaceHolder = Gtk::manage( new Gtk::MenuItem( "_Next", true ) );
  textPlaceHolder = Gtk::manage( new Gtk::MenuItem( "_Text", true ) );
  previousSearchPlaceHolder =
    Gtk::manage( new Gtk::MenuItem( "_Previous", true ) );
  searchPlaceHolder = Gtk::manage( new Gtk::MenuItem( "_Search", true ) );
  helpPlaceHolder = Gtk::manage( new Gtk::MenuItem( "_Help", true ) );
  aboutPlaceHolder = Gtk::manage( new Gtk::MenuItem( "_About", true ) );

  fileMenu = Gtk::manage( new Gtk::Menu() );
  optionsMenu = Gtk::manage( new Gtk::Menu() );
  searchMenu = Gtk::manage( new Gtk::Menu() );
  helpMenu = Gtk::manage( new Gtk::Menu() );

  menubar->append( *filePlaceHolder );
  filePlaceHolder->set_submenu( *fileMenu );
  fileMenu->append( *openPlaceHolder );
  fileMenu->append( *closePlaceHolder );

  menubar->append( *optionsPlaceHolder );
  optionsPlaceHolder->set_submenu( *optionsMenu );
  optionsMenu->append( *searchPlaceHolder );
  searchPlaceHolder->set_submenu( *searchMenu );
  searchMenu->append( *textPlaceHolder );
  searchMenu->append( *nextSearchPlaceHolder );
  searchMenu->append( *previousSearchPlaceHolder );

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
HexV::setupEvents()
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
  hexView->signal_drag_data_received().connect(
    sigc::mem_fun( *this, &HexV::openDroppedFile ) );

  // Search
  searchEntry->signal_search_changed().connect(
    sigc::mem_fun( *this, &HexV::search ) );
  searchEntry->signal_stop_search().connect(
    sigc::mem_fun( *this, &HexV::disableSearch ) );

  next->signal_clicked().connect( sigc::mem_fun( *this, &HexV::searchNext ) );
  previous->signal_clicked().connect(
    sigc::mem_fun( *this, &HexV::searchPrevious ) );
};

void
HexV::openFile()
{
  Gtk::FileChooserDialog fileDialog(
    *this, Glib::ustring( "Open File" ), Gtk::FILE_CHOOSER_ACTION_OPEN );
  fileDialog.add_button( Glib::ustring( "_Open" ), Gtk::RESPONSE_OK );
  fileDialog.add_button( Glib::ustring( "_Cancel" ), Gtk::RESPONSE_CANCEL );

  s32 response = fileDialog.run();
  fileDialog.hide();

  if ( response == Gtk::RESPONSE_OK ) {
    if ( Glib::ustring( fileDialog.get_filename() ) != file.path ) {

      textBuffer->set_text( "" );
      hexBuffer->set_text( "" );

      parseFilePath( Glib::ustring( fileDialog.get_filename() ) );
      headerBar->set_subtitle( file.name );

      processor.loadFile( file.path );
      textBuffer->set_text( *( processor.getTextBuffer() ) );
      hexBuffer->set_text( *( processor.getHexBuffer() ) );
      resetSearch();
    } else
      displayErrorMessage( "Error Opening File", "File already open" );
  }
};

void
HexV::openDroppedFile( const Glib::RefPtr<Gdk::DragContext>& context,
                       s32,
                       s32,
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

      processor.loadFile( file.path );
      textBuffer->set_text( *( processor.getTextBuffer() ) );
      hexBuffer->set_text( *( processor.getHexBuffer() ) );
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
HexV::resetSearch()
{
  searchHistoryIndex = -1;
  searchHistory.clear();
  searchBuffer->set_text( "" );
  updateSearchLabel();
}

void
HexV::search()
{
  const Glib::ustring searchedText = searchBuffer->get_text();
  Glib::RefPtr<Gtk::Adjustment> adj = scrolledWindow->get_vadjustment();

  processor.clearSearch();
  searchHistory.clear();
  indexes = processor.searchAll( searchedText );
  searchCounter = indexes.size();

  for ( auto i = 0; i < searchCounter; ++i ) {
    const f32 posInPanel =
      static_cast<u32>( indexes[i] / CHARS_PER_LINE ) * PIXELS_PER_LINE;
    searchHistory.push_back( { indexes[i], posInPanel } );
    searchHistoryIndex = 0;
  }

  if ( searchHistory.size() > 0 ) {
    updateSearchLabel();
    highlightText();
  } else {
    resetSearch();

    textBuffer->select_range( textBuffer->begin(), textBuffer->begin() );
    adj->set_value( 0.0 );
  }
};

void
HexV::searchNext()
{
  const Glib::ustring searchedText = searchBuffer->get_text();

  if ( searchedText.length() > 0 &&
       searchHistoryIndex < searchHistory.size() - 1 ) {
    ++searchHistoryIndex;
    updateSearchLabel();
    highlightText();
  }
};

void
HexV::searchPrevious()
{
  const Glib::ustring searchedText = searchBuffer->get_text();

  if ( searchedText.length() > 0 && searchHistoryIndex > 0 ) {
    --searchHistoryIndex;
    updateSearchLabel();
    highlightText();
  }
};

void
HexV::updateSearchLabel()
{
  searchLabel->set_text( std::to_string( searchHistoryIndex + 1 ) + " / " +
                         std::to_string( searchCounter ) );
}

void
HexV::highlightText()
{
  const Glib::ustring text = searchBuffer->get_text();
  const Glib::RefPtr<Gtk::Adjustment> adj = scrolledWindow->get_vadjustment();

  const Gtk::TextIter rangeStart = textBuffer->get_iter_at_offset(
    static_cast<s32>( searchHistory[searchHistoryIndex].pos ) );
  const Gtk::TextIter rangeEnd = textBuffer->get_iter_at_offset(
    static_cast<s32>( searchHistory[searchHistoryIndex].pos + text.length() ) );

  textBuffer->select_range( rangeStart, rangeEnd );
  adj->set_value( searchHistory[searchHistoryIndex].posInPanel );
};

void
HexV::disableSearch()
{
  searchBox->hide();
  if ( textBuffer ) {
    textBuffer->select_range( textBuffer->begin(), textBuffer->begin() );
  }

  resetSearch();
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
  file.name = file.path.substr( lastSlash + 1, file.path.size() - lastSlash );
};
