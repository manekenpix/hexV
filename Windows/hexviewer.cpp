#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#define ID_FILE_MENU 1
#define ID_FILE_OPEN 2
#define ID_FILE_CLOSE 3

void copyCharIntoBuffer(char *tx, char* fb, int position)
{
  for (int i = 0; i < 3; i++)
  {
    if (tx[i] == '\r')
      fb[position + i + 1] = tx[i + 1];
    
    fb[position + i] = tx[i];
  }
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

void processFile(char *fileName, HWND textControl)
{
  char *fileBuffer, *finalBuffer;
  unsigned int sizeOfFile;
  char text[4];
  void *fb, *fnb;
  
  FILE *binaryFile;
  
  binaryFile = fopen(fileName, "rb");
  if (binaryFile == NULL)
  {
    fprintf(stderr, "There was an error opening the file\n");
    
  }
  
  fseek(binaryFile, 0L, SEEK_END);
  sizeOfFile = ftell(binaryFile);
  fseek(binaryFile, 0L, SEEK_SET);
  
  fb = VirtualAlloc(0, sizeOfFile, MEM_COMMIT, PAGE_READWRITE);
  fileBuffer = (char*)fb;
  fread(fileBuffer, sizeof(char), sizeOfFile, binaryFile);
  fclose(binaryFile);
  
  int numberOfLines = sizeOfFile / 10;
  int finalSize = sizeOfFile + 50 + (numberOfLines * 45);
  
  fnb = VirtualAlloc(0, finalSize, MEM_COMMIT, PAGE_READWRITE);
  finalBuffer = (char*)fnb;
  
  char topText[] = "Position        00 01 02 03 04 05 06 07 08 09\r\n\r\n";
  
  int finalBufferIndex = 0;
  for (int i = 0; i < 49; i++)
  {
    finalBuffer[i] = topText[i];
  }
  
  finalBufferIndex = 49;
  
  int followTheCounter = 0;
  for (unsigned int i = 0; i < sizeOfFile; i++)
  {
    unsigned char lPart, hPart;
    
    hPart = (fileBuffer[i] >> 4) & 0x0F;
    lPart = (fileBuffer[i] & 0x0F);
    
    text[0] = hPart < 10 ? (hPart += 48) : (hPart += 55);
    text[1] = lPart < 10 ? (lPart += 48) : (lPart += 55);
    text[3] = '\0';
    
    if ((i + 1) != sizeOfFile && ((i + 1) % 10) == 0)
    {
      text[2] = '\r';
      text[3] = '\n';
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
    if (text[2] == '\r')
      finalBufferIndex += 4;
    else
      finalBufferIndex += 3;
    followTheCounter++;
  }
  finalBuffer[finalBufferIndex] = '\0';
  Edit_SetText(textControl, finalBuffer);
  VirtualFree(fileBuffer, 0, MEM_RELEASE);
  VirtualFree(finalBuffer, 0, MEM_RELEASE);
}

LRESULT CALLBACK MainWindowCallBack(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
  LRESULT result = 0;
  switch (message)
  {
    case WM_CLOSE:
    {
      PostQuitMessage(0);
    }break;
    
    default:
    {
      result = DefWindowProc(window, message, wParam, lParam);
      
    }break;
    
  }
  return result;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  WNDCLASS WindowsClass = {};
  
  WindowsClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  WindowsClass.lpfnWndProc = MainWindowCallBack;
  WindowsClass.lpszClassName = "window";
  WindowsClass.lpszMenuName = MAKEINTRESOURCE(ID_FILE_MENU);
  
  HMENU hMenubar = CreateMenu();
  HMENU hMenu = CreateMenu();
  
  AppendMenu(hMenu, MF_STRING, ID_FILE_OPEN, "&Open");
  AppendMenu(hMenu, MF_STRING, ID_FILE_CLOSE, "&Close");
  AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu, "&File");
  
  MSG message;
  
  if (!RegisterClass(&WindowsClass))
    return 1;
  
  HWND MainWindowHandle = CreateWindowEx(WS_EX_CONTROLPARENT, WindowsClass.lpszClassName, "My window", WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN, 300, 300, 430, 500, 0, hMenubar, hInstance, NULL);
  
  HWND textBox = CreateWindowEx(0, "EDIT", 0, ES_MULTILINE | WS_VISIBLE | WS_VSCROLL | WS_CHILD, 5, 5, 410, 435, MainWindowHandle, 0, hInstance, NULL);
  
  HFONT hFont = CreateFont(17, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                           CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Courier New"));
  
  SendMessage(textBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
  
  
  while (GetMessage(&message, 0, 0, 0))
  {
    switch (message.message)
    {
      case WM_COMMAND:
      {
        if (message.wParam == ID_FILE_CLOSE)
          PostQuitMessage(0);
        else if (message.wParam == ID_FILE_OPEN)
        {
          CHAR fileName[256] = {};
          
          OPENFILENAME dialogStruct = {};
          
          dialogStruct.lStructSize = sizeof(OPENFILENAME);
          dialogStruct.hwndOwner = MainWindowHandle;
          dialogStruct.lpstrFile = fileName;
          dialogStruct.nMaxFile = 256;
          
          GetOpenFileName(&dialogStruct);
          processFile(fileName, textBox);
          
        }
        
      }break;
    }
    
    TranslateMessage(&message);
    DispatchMessage(&message);
  }
  return message.wParam;
}
