#include "HexV.h"

int main(int argc, char *argv[])
{
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "Hexviewer");
  
  HexV hexV;
  
  return app->run(hexV);
}