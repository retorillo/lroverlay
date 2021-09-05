#define UNICODE

#include <windows.h>
#include <list>
#include <memory>
#include <string>
#include <tchar.h>
#include <shellapi.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")

#ifdef UNICODE
  #define tstring wstring
  #define tcsicmp _wcsicmp
  #define vstprintf vswprintf
  #define CommandLineToArgv CommandLineToArgvW
#else
  #define tstring string
  #define tcsicmp _stricmp
  #define vstprintf vsprintf
  #define CommandLineToArgv CommandLineToArgvA
  // TODO: implement CommandLineArgvA
#endif

using namespace std;

#define APP_CLASSNAME TEXT("lroverlay")
#define APP_TITLE APP_CLASSNAME
#define APP_OPACITY 128 // 0-255

#define APP_MARGIN 5 // %
#define APP_MARGIN_MAX 30 // %
#define APP_MARGIN_MIN 0 // %
#define APP_SIZE 5 // %
#define APP_SIZE_MAX 20 // %
#define APP_SIZE_MIN 1 // %
#define APP_LEFTTEXT TEXT("R")
#define APP_RIGHTTEXT TEXT("L")


#ifdef UNICODE
  #define winmain wWinMain
#else
  #define winmain WinMain
#endif

enum PLACEMENT {
  top,
  middle,
  bottom, 
};

struct CONFIG {
  int size;
  int margin;
  PLACEMENT place;
  tstring ltext;
  tstring rtext;
};

list<RECT> monitors();
LRESULT redraw(HWND, const CONFIG*);
int WINAPI winmain(HINSTANCE i, HINSTANCE p, PTSTR cl, int cs);

struct deletor {
private:
  const void* ptr;
  void(* del)(const void*);
public:
  deletor(const void* p, void(* d)(const void* ptr)) : ptr(p), del(d) {};
  ~deletor() { if (del) del(ptr); del = NULL; }
};

class error {
  tstring _what;
  public:
    error(tstring what) : _what(what) { }
    tstring what() { return _what; }
};
