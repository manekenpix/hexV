#ifndef HEXV_H
#define HEXV_H

#include "Processor/Processor.h"
#include "types.h"
#include <cstdint>
#include <fstream>
#include <glibmm/ustring.h>
#include <gtkmm.h>

class HexV : public Gtk::Window
{
  const size_t WIDTH = 475;
  const size_t HEIGHT = 700;
  const u8 CHARS_PER_LINE = 17;
  const u32 PIXELS_PER_LINE = 14;

  Processor processor;

  typedef struct FileType
  {
    Glib::ustring name, path;
    std::string raw;
  } FileType;

  FileType file;

  // Window
  Gtk::HeaderBar* headerBar;
  Gtk::MenuBar* menubar;
  Gtk::MenuItem* filePlaceHolder;
  Gtk::MenuItem* openPlaceHolder;
  Gtk::MenuItem* closePlaceHolder;
  Gtk::MenuItem* optionsPlaceHolder;
  Gtk::MenuItem* textPlaceHolder;
  Gtk::MenuItem* nextSearchPlaceHolder;
  Gtk::MenuItem* previousSearchPlaceHolder;
  Gtk::MenuItem* searchPlaceHolder;
  Gtk::MenuItem* helpPlaceHolder;
  Gtk::MenuItem* aboutPlaceHolder;
  Gtk::Menu* fileMenu;
  Gtk::Menu* optionsMenu;
  Gtk::Menu* searchMenu;
  Gtk::Menu* helpMenu;
  Gtk::Grid* grid;
  Gtk::Box* bigBox;
  Gtk::Box* searchBox;

  // Search
  Gtk::Button* next;
  Gtk::Image* nextIcon;
  Gtk::Button* previous;
  Gtk::Image* previousIcon;
  Gtk::Label* searchLabel;
  Gtk::SearchEntry* searchEntry;
  Glib::RefPtr<Gtk::EntryBuffer> searchBuffer;
  std::vector<std::string::size_type> indexes;
  u32 searchHistoryIndex = 0;
  u32 searchCounter = 0;

  typedef struct SearchHistoryType
  {
    std::string::size_type pos;
    f32 posInPanel;

    SearchHistoryType( std::string::size_type _pos, f32 _posInPanel )
      : pos{ _pos }
      , posInPanel{ _posInPanel } {};
  } SearchHistoryType;

  std::vector<SearchHistoryType> searchHistory;

  // Panels
  Gtk::ScrolledWindow* scrolledWindow;
  Gtk::TextView* textView;
  Gtk::TextView* hexView;
  Glib::RefPtr<Gtk::CssProvider> cssProvider;
  Glib::RefPtr<Gtk::StyleContext> styleContext;
  Glib::RefPtr<Gtk::TextBuffer> textBuffer;
  Glib::RefPtr<Gtk::TextBuffer> hexBuffer;

  // Targets:
  std::vector<Gtk::TargetEntry> listTargets;

  // Helper
  void parseFilePath( std::string );
  void displaySearchBox();
  void search();
  void searchNext();
  void searchPrevious();
  void updateSearchLabel();
  void disableSearch();
  void resetSearch();
  void highlightText();
  void openFile();
  void openDroppedFile( const Glib::RefPtr<Gdk::DragContext>& context,
                        s32,
                        s32,
                        const Gtk::SelectionData& selection_data,
                        guint,
                        guint time );
  void setupWindow();
  void setupContainers();
  void setupSearchBox();
  void setupMenu();
  void setupPanels();
  void setupEvents();
  void about();
  void displayErrorMessage( const Glib::ustring& text,
                            const Glib::ustring& subtext );
  void exit() { hide(); };

public:
  HexV();
  ~HexV(){};
};

#endif
