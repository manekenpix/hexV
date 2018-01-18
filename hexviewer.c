#include <stdlib.h>
#include <gtk/gtk.h>

static int processFile(GtkWindow *widget, gpointer VB)
{
    GtkWidget *dialogue;
    int buttonClicked;
    unsigned char *fileBuffer;
    unsigned int sizeOfFile;
    GtkTextBuffer*viewerBuffer = VB;
    
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
            printf("There was an error opening the file\n");
            return 0;
        }
        
        fseek(binaryFile, 0L, SEEK_END);
        sizeOfFile = ftell(binaryFile);
        fseek(binaryFile, 0L, SEEK_SET);
        fileBuffer = (unsigned char*)malloc(sizeOfFile);
        fread(fileBuffer, sizeof(char), sizeOfFile, binaryFile);
        fclose(binaryFile);
        
        //gtk_text_buffer_set_text(viewerBuffer, fileName, -1);
        gtk_text_buffer_set_text(viewerBuffer, "Position   Value\n           00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF\n", -1);
        char text[20];
        for (unsigned int i = 0; i < sizeOfFile; i++)
        {
            if (((i + 1) != sizeOfFile) && (((i + 1) % 16) == 0))
                snprintf(text, sizeof(text), " %02X\n", fileBuffer[i]);
            else if ((i % 16) == 0)
                snprintf(text, sizeof(text), "%010i %02X", i, fileBuffer[i]);
            else
                snprintf(text, sizeof(text), " %02X", fileBuffer[i]);
            gtk_text_buffer_insert_at_cursor(viewerBuffer, text, -1);
        }
        free(fileBuffer);
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
    GtkWidget *textViewer;
    GtkWidget *textViewerBuffer;
    GtkWidget *menuBar;
    GtkWidget *fileMenu;
    GMenuItem *fileItem;
    GtkWidget *openFile;
    GtkWidget *closeApp;
    
    gtk_init(&argc, &argv);
    
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 500); 
    gtk_window_set_title(GTK_WINDOW(window), "Hexadecimal Viewer");
    
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