#include "lroverlay.h"

list<RECT> monitors() {
  list<RECT> rects;
  EnumDisplayMonitors(NULL, NULL, [](HMONITOR m, HDC h, LPRECT r, LPARAM l) -> BOOL {
    (reinterpret_cast<list<RECT>*>(l))->push_back(*r);
    return TRUE;
  }, reinterpret_cast<LPARAM>(&rects));
  return rects;
}

//TODO: use Gdi+
LRESULT redraw(HWND h, const CONFIG* cfg) {
  RECT cr;
  RECT wr;
  GetClientRect(h, &cr);
  GetWindowRect(h, &wr);
  int W = wr.right - wr.left;
  int H = wr.bottom - wr.top;

  RECT ir = wr;
  InflateRect(&ir, -round(W * (cfg->margin / 100.0f)), -round(H * (cfg->margin / 100.0f)));

  int boxsize = round((ir.right - ir.left) * (cfg->size / 100.0f));
 
  // TODO: cfg->place 
  RECT lr = { ir.left, ir.top, ir.left + boxsize, ir.top + boxsize };
  RECT rr = { ir.right - boxsize, ir.top, ir.right, ir.top + boxsize};

  HDC hsdc = GetDC(NULL);
  HDC hdc  = GetDC(h);
  HDC hbdc = CreateCompatibleDC(hdc);
  HBITMAP bmp = CreateCompatibleBitmap(hsdc, W, H);
  HGDIOBJ oldbmp = SelectObject(hbdc, reinterpret_cast<HGDIOBJ>(bmp));
  HBRUSH brush = CreateSolidBrush(RGB(255,255,255));
  FillRect(hbdc, &lr, brush);
  FillRect(hbdc, &rr, brush);

  HFONT font = CreateFont(boxsize, 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH,
    TEXT("Consolas"));
  HGDIOBJ oldfont = SelectObject(hbdc, reinterpret_cast<HGDIOBJ>(font));

  DrawText(hbdc, cfg->ltext.c_str(), -1, &lr, DT_CENTER | DT_VCENTER);
  DrawText(hbdc, cfg->rtext.c_str(), -1, &rr, DT_CENTER | DT_VCENTER);

  UpdateLayeredWindow(h, hsdc, &(POINT() = {wr.left, wr.top}),
    &(SIZE() = {W, H}), hbdc, &(POINT() = {0, 0}), NULL,
    &(BLENDFUNCTION() = { AC_SRC_OVER, 0, APP_OPACITY, AC_SRC_ALPHA }), ULW_ALPHA);

  SelectObject(hbdc, oldfont);
  SelectObject(hbdc, oldbmp);
  DeleteObject(font);
  DeleteObject(brush);
  DeleteObject(bmp);
  DeleteDC(hbdc);
  ReleaseDC(h, hdc);
  ReleaseDC(NULL, hsdc);
  return 0;
}


TCHAR mkerror_buf[MAX_PATH];
error mkerror(const TCHAR* fmt, ...) {
  try {
    va_list args;
    va_start(args, fmt);
    vstprintf(mkerror_buf, fmt, args);
    va_end(args); 
    return error(mkerror_buf);
  }
  catch (...) {
    return error(fmt);
  }
}

PLACEMENT parseopt(tstring text, const TCHAR* optname) {
  if (tcsicmp(text.c_str(), TEXT("top")) == 0) return PLACEMENT::top;
  if (tcsicmp(text.c_str(), TEXT("middle")) == 0) return PLACEMENT::middle;
  if (tcsicmp(text.c_str(), TEXT("bottom")) == 0) return PLACEMENT::bottom;
  throw mkerror(TEXT("option \"%s\" textue must be one of the following: top, middle, bottom"), optname);
}

int parseopt(tstring text, int min, int max, const TCHAR* optname) {
  try {
    int val = stoi(text);
    if (val > max || val < min)
      throw NULL;
    return val;
  }
  catch (...) {
    throw mkerror(TEXT("option \"%s\" value must be between %d - %d"), optname, min, max);
  }
}

CONFIG config() {
  CONFIG cfg;
  cfg.size = APP_SIZE;
  cfg.margin = APP_MARGIN;
  cfg.place = PLACEMENT::top;
  cfg.ltext = APP_LEFTTEXT;
  cfg.rtext = APP_RIGHTTEXT;
  TCHAR* opt = NULL;
  TCHAR* val = NULL;
  int argn;
  LPWSTR* argv = CommandLineToArgv(GetCommandLine(), &argn);
  if (!argv)
    throw error(TEXT("cannot parse command line arguments"));
  deletor deletor_argv(argv, [](const void* ptr) -> void { LocalFree((HLOCAL)ptr); });
  for (int c = 1; c < argn; c++) {
    if (argv[c][0] == TEXT('-')) {
      if (opt)
        throw mkerror(TEXT("option \"%s\" requires its value"), opt);
      opt = argv[c] + 1;
    }
    else {
      // TODO: implement --opacity and -o switch, CHECK click behavior when opacity = 100
      // TODO: implement --screen and -S switch
      // TODO: compare case-sensitive when single char option for the above
      // TODO: implement --invert and -i switch
      val = argv[c];
      if (!opt)
        throw mkerror(TEXT("value \"%s\" requires hyphended option name before it"), val);
      else if (!tcsicmp(opt, TEXT("-size")) || !tcsicmp(opt, TEXT("s")))
        cfg.size = parseopt(val, APP_SIZE_MIN, APP_SIZE_MAX, opt);
      else if (!tcsicmp(opt, TEXT("-margin")) || !tcsicmp(opt, TEXT("m")))
        cfg.margin = parseopt(val, APP_MARGIN_MIN, APP_MARGIN_MAX, opt);
      else if (!tcsicmp(opt, TEXT("-left")) || !tcsicmp(opt, TEXT("l")))
        cfg.ltext = val;
      else if (!tcsicmp(opt, TEXT("-right")) || !tcsicmp(opt, TEXT("r")))
        cfg.ltext = val;
      else if (!tcsicmp(opt, TEXT("-place")) || !tcsicmp(opt, TEXT("p")))
        cfg.place = parseopt(val, opt);
      else
        throw mkerror(TEXT("unknown option: \"-%s\""), opt);
      opt = NULL;
    }
  }
  return cfg;
}

CONFIG cfg;
int WINAPI winmain(HINSTANCE i, HINSTANCE p, PTSTR cl, int cs) {
  try {
    cfg = config();
    WNDCLASSEX wc = { };
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.hInstance = i;
    wc.lpszClassName = APP_CLASSNAME;
    wc.lpfnWndProc = [](HWND h, UINT m, WPARAM w, LPARAM l) -> LRESULT  {
      switch (m) {
        case WM_CREATE:
          redraw(h, &cfg);
          return 0;
        case WM_DESTROY:
          PostQuitMessage(0);
          return 0; 
        case WM_PAINT:
          return 0;
        default:
          return DefWindowProc(h, m, w, l);
      }
    };
    ATOM atom = RegisterClassEx(&wc);
    if (!atom)
      throw error(TEXT("cannot register window class"));
    
    list<RECT> rects = monitors();
    RECT mm = rects.front();

    HWND h = CreateWindowEx(WS_EX_APPWINDOW|WS_EX_LAYERED|WS_EX_TOPMOST|WS_EX_TRANSPARENT,
      APP_CLASSNAME, APP_TITLE, WS_POPUP,
      mm.left, mm.top, mm.right - mm.left, mm.bottom - mm.top,
      NULL, NULL, i, NULL);
    if (!h)
      throw error(TEXT("cannot create window"));
    ShowWindow(h, cs);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    return 0;
  }
  catch (error e) {
    MessageBox(NULL, e.what().c_str(), APP_TITLE, MB_OK|MB_ICONERROR|MB_DEFBUTTON1);
    return GetLastError();
  }
}
