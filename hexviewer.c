#include <gtk/gtk.h>

// compile with this 
// gcc -Wall -ggdb hexviewer.c -o hexviewer `pkg-config --libs --cflags gtk+-3.0`

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
    for (int i = 1; i < exp; i++)
      result *= (long long int)base;
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

int processFile(GtkWindow *widget, gpointer VB)
{
  GtkWidget *dialogue;
  int buttonClicked;
  char *fileBuffer;
  unsigned int sizeOfFile;
  GtkTextBuffer *viewerBuffer = VB;
  char text[3];
  
  dialogue = gtk_file_chooser_dialog_new("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, ("_Cancel"), GTK_RESPONSE_CANCEL, ("_Accept"),  GTK_RESPONSE_ACCEPT, NULL);
  
  buttonClicked = gtk_dialog_run(GTK_DIALOG(dialogue));
  if (buttonClicked == GTK_RESPONSE_ACCEPT)
  {
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialogue);
    char *fileName = gtk_file_chooser_get_filename(chooser);
    FILE *binaryFile;
    
    gtk_widget_destroy(dialogue);
    binaryFile = fopen(fileName, "rb"); 
    if (binaryFile == NULL){
      fprintf(stderr, "There was an error opening the file\n");
      return 1;
    }
    
    fseek(binaryFile, 0L, SEEK_END);
    sizeOfFile = ftell(binaryFile);
    fseek(binaryFile, 0L, SEEK_SET);
    fileBuffer = (char*)malloc(sizeOfFile);
    fread(fileBuffer, sizeof(char), sizeOfFile, binaryFile);
    fclose(binaryFile);
    
    int numberOfLines = sizeOfFile / 10;
    int finalSize = sizeOfFile + 50 + (numberOfLines * 45);
    
    char *finalBuffer = (char*)malloc(finalSize);
    
    char *topText = "Position        00 01 02 03 04 05 06 07 08 09\n\n";
    
    int finalBufferIndex = 0;
    //printf("The size of the file is %d and the new buffer is %d\n", sizeOfFile, finalSize);
    for (int i = 0; i < 47; i++)
    {
      finalBuffer[i] = topText[i];
    }
    
    finalBufferIndex = 47;
    
    int followTheCounter = 0;
    for (int i = 0; i < sizeOfFile; i++)
    {
      unsigned char lPart, hPart;
      
      hPart = (fileBuffer[i] >> 4) & 0x0F;
      lPart = (fileBuffer[i] & 0x0F);
      
      text[0] = hPart < 10 ? (hPart += 48) : (hPart += 55);
      text[1] = lPart < 10 ? (lPart += 48) : (lPart += 55);
      
      if ((i + 1) != sizeOfFile && ((i + 1) % 10) == 0)
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
    //printf("final Buffer Index %d, i is %d\n", finalBufferIndex, followTheCounter);
    finalBuffer[finalBufferIndex] = '\0';
    gtk_text_buffer_set_text(viewerBuffer, finalBuffer, finalBufferIndex);
    free(fileBuffer);
    free(finalBuffer);
    return 0;
  }
  else
  {
    gtk_widget_destroy(dialogue);
    return 0;
  }
}

int main(int argc, char *argv[])
{
  GtkWidget *window;
  GtkBox *mainBox;
  GtkWidget *scrolledWindow;
  GtkTextView *textViewer;
  GtkTextBuffer *textViewerBuffer;
  GtkWidget *menuBar;
  GtkWidget *dropDownMenu;
  GMenuItem *barItem;
  GtkWidget *openFile;
  GtkWidget *closeApp;
  
  gtk_init(&argc, &argv);
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 500); 
  gtk_window_set_title(GTK_WINDOW(window), "Hexadecimal Viewer");
  
  mainBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
  
  menuBar = gtk_menu_bar_new();
  dropDownMenu = gtk_menu_new();
  barItem = gtk_menu_item_new_with_label("File");
  openFile = gtk_menu_item_new_with_label("Open");
  closeApp = gtk_menu_item_new_with_label("Close");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(barItem), dropDownMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), GTK_WIDGET(barItem));
  gtk_menu_shell_append(GTK_MENU_SHELL(dropDownMenu), openFile);
  gtk_menu_shell_append(GTK_MENU_SHELL(dropDownMenu), closeApp);
  
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
  
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(closeApp, "activate", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(openFile, "activate", G_CALLBACK(processFile), textViewerBuffer);
  
  gtk_widget_show_all(window);
  
  gtk_main();
  
  return 0;
}