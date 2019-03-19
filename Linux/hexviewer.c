#include <gtk/gtk.h>

void copyCharIntoBuffer(char *tx, char* fb, int position)
{
  for (int i = 0; i < 3; i++)
    fb[position + i] = tx[i];
}

long long int powForInts(int base, int exp)
{
  long long int result = base;
  if (exp == 0)
    result = 1;
  else
  {
    base = (long long int)base;
    for (int i = 1; i < exp; i++)
      result *= base;
  }
  return result;
}

void copyPositionIntoBuffer(char *fB, int position, int index)
{
  for (int i = 0; i < 15; i++)
  {
    fB[position + i] = (((long long int)index % powForInts(10, 15 - i)) / powForInts(10, 14 - i)) + 48;
  }
}

char* processFile(char* fileBuffer, int *length)
{
  char text[3];
  int numberOfLines = *length / 10;
  int finalSize = *length + 50 + (numberOfLines * 45);
  
  char *finalBuffer = (char*)malloc(finalSize);
  
  char *topText = "Position        00 01 02 03 04 05 06 07 08 09\n\n";
  
  int finalBufferIndex = 0;
  for (int i = 0; i < 47; i++)
  {
    finalBuffer[i] = topText[i];
  }
  
  finalBufferIndex = 47;
  
  int followTheCounter = 0;
  for (int i = 0; i < *length; i++)
  {
    unsigned char lPart, hPart;
    
    hPart = (fileBuffer[i] >> 4) & 0x0F;
    lPart = (fileBuffer[i] & 0x0F);
    
    text[0] = hPart < 10 ? (hPart += 48) : (hPart += 55);
    text[1] = lPart < 10 ? (lPart += 48) : (lPart += 55);
    
    if ((i + 1) != *length && ((i + 1) % 10) == 0)
    {
      text[2] = '\n';
    }
    
    else if ((i % 10) == 0)
    {
      text[2] = ' ';
      copyPositionIntoBuffer(finalBuffer, finalBufferIndex, i);
      finalBufferIndex += 15;
      finalBuffer[finalBufferIndex] = 32;
      finalBufferIndex++;
    }
    else
    {
      text[2] = ' ';
    }
    copyCharIntoBuffer(text, finalBuffer, finalBufferIndex);
    finalBufferIndex += 3;
    followTheCounter++;
  }
  finalBuffer[finalBufferIndex] = '\0';
  *length = finalBufferIndex;
  return finalBuffer;
}

void getFileHandler(GtkWindow *widget, gpointer VB)
{
  GtkWidget *dialogue;
  int buttonClicked;
  GtkTextBuffer *viewerBuffer = VB;
  
  dialogue = gtk_file_chooser_dialog_new("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, ("_Cancel"), GTK_RESPONSE_CANCEL, ("_Accept"),  GTK_RESPONSE_ACCEPT, NULL);
  
  buttonClicked = gtk_dialog_run(GTK_DIALOG(dialogue));
  if (buttonClicked == GTK_RESPONSE_ACCEPT)
  {
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialogue);
    char *fileName = gtk_file_chooser_get_filename(chooser);
    printf("%s\n", gtk_file_chooser_get_filename(chooser));
    
    gtk_widget_destroy(dialogue);
    FILE *file = fopen(fileName, "rb"); 
    if (file == NULL){
      fprintf(stderr, "There was an error opening the file\n");
    }
    else
    {
      char *fileBuffer;
      int *length = (int*)malloc(4);
      fseek(file, 0L, SEEK_END);
      *length = ftell(file);
      fseek(file, 0L, SEEK_SET);
      fileBuffer = (char*)malloc(*length);
      fread(fileBuffer, sizeof(char), *length, file);
      fclose(file);
      
      char* finalBuffer = processFile(fileBuffer, length);
      
      gtk_text_buffer_set_text(viewerBuffer, finalBuffer, *length);
      free(fileBuffer);
      free(finalBuffer);
      free(length);
    }
  }
  else
  {
    gtk_widget_destroy(dialogue);
  }
}

void processDrop(GtkWindow *widget,  GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer VB)
{
  GtkTextBuffer *viewerBuffer = VB;
  
  gint selectionLength = gtk_selection_data_get_length(data);
  const gchar *temp = (gchar*)gtk_selection_data_get_data(data);
  
  gchar* fileName = (gchar*)malloc(selectionLength);
  for (int i = 7; i < selectionLength - 1; ++i)
  {
    fileName[i- 7] = temp[i];
  }
  fileName[selectionLength - 9] = '\0';
  
  FILE *file = fopen(fileName, "rb"); 
  if (file == NULL){
    fprintf(stderr, "There was an error opening the file\n");
  }
  else
  {
    char *fileBuffer;
    int *length = (int*)malloc(4);
    fseek(file, 0L, SEEK_END);
    *length = ftell(file);
    fseek(file, 0L, SEEK_SET);
    fileBuffer = (char*)malloc(*length);
    fread(fileBuffer, sizeof(char), *length, file);
    fclose(file);
    
    char* finalBuffer = processFile(fileBuffer, length);
    
    gtk_text_buffer_set_text(viewerBuffer, finalBuffer, *length);
    free(fileBuffer);
    free(finalBuffer);
    free(length);
  }
  
}

void showAbout(GtkWindow *widget)
{
  char firstAuthor[] = "Josue Quilon Barrios";
  char *authors[2]; 
  authors[0] = firstAuthor;
  authors[1] = NULL;
  
  gtk_show_about_dialog(NULL, "program-name", "Hexviewer", "comments", "Software created to view file content in hexadecimal", "authors", authors,"title", "About Hexaviewer", "version", "0.0.3", NULL);
}

int main(int argc, char *argv[])
{
  GtkWidget *window;
  GtkBox *mainBox;
  GtkWidget *scrolledWindow;
  GtkTextView *textViewer;
  GtkTextBuffer *textViewerBuffer;
  GtkWidget *menuBar;
  GtkWidget *helpMenu, *fileMenu;
  GMenuItem *fileItem, *helpItem;
  GtkWidget *openFile, *closeApp, *aboutHelp;
  
  gtk_init(&argc, &argv);
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 500); 
  gtk_window_set_title(GTK_WINDOW(window), "Hexadecimal Viewer");
  
  FILE *icon = fopen("hv.png", "rb"); 
  if (icon  != NULL)
  {
    fclose(icon);
    gtk_window_set_icon_from_file(GTK_WINDOW(window), "hv.png", NULL);
  }
  
  mainBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
  
  menuBar = gtk_menu_bar_new();
  fileMenu = gtk_menu_new();
  fileItem = gtk_menu_item_new_with_label("File");
  openFile = gtk_menu_item_new_with_label("Open");
  closeApp = gtk_menu_item_new_with_label("Close");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileItem), fileMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), GTK_WIDGET(fileItem));
  gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), openFile);
  gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), closeApp);
  
  helpMenu = gtk_menu_new();
  helpItem = gtk_menu_item_new_with_label("Help");
  aboutHelp = gtk_menu_item_new_with_label("About");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpItem), helpMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), GTK_WIDGET(helpItem));
  gtk_menu_shell_append(GTK_MENU_SHELL(helpMenu), aboutHelp);
  
  scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
  textViewer = gtk_text_view_new();
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(textViewer), TRUE);
  textViewerBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textViewer));
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textViewer), GTK_WRAP_WORD);
  
  gtk_container_set_border_width(GTK_CONTAINER (scrolledWindow), 5);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrolledWindow),GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  gtk_container_add(GTK_CONTAINER(scrolledWindow), textViewer);
  gtk_box_pack_start(mainBox, menuBar, FALSE, FALSE, 0);
  gtk_box_pack_start(mainBox, scrolledWindow, TRUE, TRUE, 0);
  gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(mainBox));
  
  GtkTargetEntry *targetEntry;
  targetEntry = gtk_target_entry_new("text/uri-list", 0, 0);
  
  gtk_drag_dest_set(window, GTK_DEST_DEFAULT_DROP, targetEntry, 1, GDK_ACTION_COPY);
  
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(closeApp, "activate", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(openFile, "activate", G_CALLBACK(getFileHandler), textViewerBuffer);
  g_signal_connect(aboutHelp, "activate", G_CALLBACK(showAbout), NULL);
  g_signal_connect(textViewer, "drag-data-received", G_CALLBACK(processDrop), textViewerBuffer);
  
  gtk_widget_show_all(window);
  
  gtk_main();
  
  return 0;
}