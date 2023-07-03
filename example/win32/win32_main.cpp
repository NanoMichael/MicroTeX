// clang-format off
#include "graphic_win32.h"
#include "microtex.h"
#include "samples.h"
#include "utils/string_utils.h"

#include <comdef.h>
#include <gdiplus.h>
#include <windows.h>
// clang-format off

#define ID_SETTER 256
#define ID_CANVAS 512
#define ID_EDITBOX 1024
#define ID_BUTTON_SIZE 2048
#define ID_BUTTON_NEXT 4096

#define BUTTON_WIDTH 180
#define BUTTON_HEIGHT 35
#define EDITOR_WIDTH 640

using namespace std;
using namespace microtex;
using namespace Gdiplus;

microtex::Render* _render = nullptr;
int _size = 26;
color _color = 0xff424242;
Samples* _samples;

HWND hEditor, hBtnSize, hBtnNext, hCanvas, hEditSize;
WNDPROC editorProc, setterProc;

void RegisterCanvas();

void CreateCtrl(HINSTANCE, HWND);

void ResizeCtrl(HWND);

void HandleOK();

void HandleNext();

void HandleSize();

void RenderFormula(HWND, HDC);

void init();

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK CanvasProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK EditorProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK SetterProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow) {
  HWND hWnd;
  MSG msg;
  WNDCLASS wndClass;
  GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;

  // Initialize GDI+.
  GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
  init();

  RegisterCanvas();

  wndClass.style = CS_HREDRAW | CS_VREDRAW;
  wndClass.lpfnWndProc = WndProc;
  wndClass.cbClsExtra = 0;
  wndClass.cbWndExtra = 0;
  wndClass.hInstance = hInstance;
  wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
  wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wndClass.lpszMenuName = nullptr;
  wndClass.lpszClassName = TEXT("MicroTeX");

  RegisterClass(&wndClass);

  hWnd = CreateWindow(
    TEXT("MicroTeX"),     // window class name
    TEXT("MicroTeX"),     // window caption
    WS_OVERLAPPEDWINDOW,  // window style
    0,                    // initial x position
    0,                    // initial y position
    780,                  // initial x size
    480,                  // initial y size
    nullptr,              // parent window handle
    nullptr,              // window menu handle
    hInstance,            // program instance handle
    nullptr               // creation parameters
  );

  CreateCtrl(hInstance, hWnd);
  ShowWindow(hWnd, SW_SHOWMAXIMIZED);
  UpdateWindow(hWnd);

  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  delete _render;
  delete _samples;
  MicroTeX::release();
  GdiplusShutdown(gdiplusToken);
  return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_DESTROY: PostQuitMessage(0); return 0;
    case WM_COMMAND:
      if (wParam == ID_BUTTON_SIZE) {
        HandleSize();
      } else if (wParam == ID_BUTTON_NEXT) {
        HandleNext();
      }
      return 0;
    case WM_SIZE: ResizeCtrl(hWnd); return 0;
    default: return DefWindowProc(hWnd, message, wParam, lParam);
  }
}  // WndProc

void init() {
  if (__argc < 5) {
    fprintf(
      stderr,
      "Required options:\n"
      "  <clm data file>\n"
      "  <math font file>\n"
      "  <samples file>\n"
    );
    exit(1);
  }
  const FontSrcFile math{__argv[2], __argv[3]};
  MicroTeX::init(math);
  PlatformFactory::registerFactory("gdi", std::make_unique<PlatformFactory_gdi>());
  PlatformFactory::activate("gdi");
  _samples = new Samples(__argv[4]);
  _render = MicroTeX::parse(
    "\\text{Hello from \\MicroTeX, have fun! Press Ctrl + Enter on editor to show "
    "formulas.}",
    720,
    _size,
    _size / 3.f,
    _color
  );
}

void HandleNext() {
  delete _render;
  RECT r;
  GetClientRect(hCanvas, &r);
  _render = MicroTeX::parse(_samples->next(), r.right - r.left, _size, _size / 3.f, _color);
  InvalidateRect(hCanvas, nullptr, TRUE);
  UpdateWindow(hCanvas);
}

void HandleOK() {
  int len = GetWindowTextLengthA(hEditor);
  char* data = new char[len + 10];
  GetWindowTextA(hEditor, data, len + 10);
  std::string txt = data;
  delete _render;
  delete[] data;
  RECT r;
  GetClientRect(hCanvas, &r);
  _render = MicroTeX::parse(txt, r.right - r.left, _size, _size / 3.f, _color);
  InvalidateRect(hCanvas, nullptr, TRUE);
  UpdateWindow(hCanvas);
}

void HandleSize() {
  if (_render == nullptr) return;
  int len = GetWindowTextLength(hEditSize);
  if (len == 0) return;
  char* txt = new char[len + 10];
  GetWindowText(hEditSize, txt, len + 10);
  string x = txt;
  microtex::valueOf(x, _size);
  _render->setTextSize(_size);
  InvalidateRect(hCanvas, nullptr, TRUE);
  UpdateWindow(hCanvas);
  delete[] txt;
}

void RenderFormula(HWND hwnd, HDC hdc) {
  // draw rectangle
  RECT r;
  GetClientRect(hwnd, &r);
  Graphics g(hdc);
  Graphics2D_win32 g2(&g);
  g2.drawRect(r.left, r.top, r.right - r.left - 1, r.bottom - r.top - 1);
  // draw formula
  if (_render != nullptr) {
    _render->draw(g2, 10, 10);
  }
}

void CreateCtrl(HINSTANCE hInst, HWND hwnd) {
  // create first, relayout through message WM_SIZE
  int l = 0, t = 0, w = 10, h = 10;
  // edit-box
  hEditor = CreateWindowEx(
    WS_EX_TOPMOST,
    "edit",
    nullptr,
    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
    l,
    t,
    w,
    h,
    hwnd,
    (HMENU)ID_EDITBOX,
    hInst,
    nullptr
  );
  HFONT hf = CreateFont(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Consolas");
  SendMessage(hEditor, WM_SETFONT, (WPARAM)hf, 0);
  SetFocus(hEditor);
  editorProc = (WNDPROC)SetWindowLongPtr(hEditor, GWLP_WNDPROC, (LONG_PTR)EditorProc);
  // size setter
  hEditSize = CreateWindowEx(
    WS_EX_TOPMOST,
    "edit",
    nullptr,
    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_NUMBER,
    l,
    t,
    w,
    h,
    hwnd,
    (HMENU)ID_SETTER,
    hInst,
    nullptr
  );
  SendMessage(hEditSize, WM_SETFONT, (WPARAM)hf, 0);
  setterProc = (WNDPROC)SetWindowLongPtr(hEditSize, GWLP_WNDPROC, (LONG_PTR)SetterProc);
  // button size
  hBtnSize = CreateWindowEx(
    WS_EX_TOPMOST,
    "Button",
    nullptr,
    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    l,
    t,
    w,
    h,
    hwnd,
    (HMENU)ID_BUTTON_SIZE,
    hInst,
    nullptr
  );
  SendMessage(hBtnSize, WM_SETFONT, (WPARAM)hf, 0);
  SendMessage(hBtnSize, WM_SETTEXT, (WPARAM) nullptr, (LPARAM)("Set Text Size"));
  // button next
  hBtnNext = CreateWindowEx(
    WS_EX_TOPMOST,
    "Button",
    nullptr,
    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    l,
    t,
    w,
    h,
    hwnd,
    (HMENU)ID_BUTTON_NEXT,
    hInst,
    nullptr
  );
  SendMessage(hBtnNext, WM_SETFONT, (WPARAM)hf, 0);
  SendMessage(hBtnNext, WM_SETTEXT, (WPARAM) nullptr, (LPARAM)("Next Example"));
  // canvas
  hCanvas = CreateWindowEx(
    WS_EX_TOPMOST,
    "canvas",
    nullptr,
    WS_CHILD | WS_VISIBLE,
    l,
    t,
    w,
    h,
    hwnd,
    (HMENU)ID_CANVAS,
    hInst,
    nullptr
  );
}

void ResizeCtrl(HWND hwnd) {
  RECT r;
  GetClientRect(hwnd, &r);
  const int margin = 10;
  // editor
  int w = EDITOR_WIDTH, h = BUTTON_HEIGHT;
  int l = r.right - w - margin, t = r.top + margin;
  h = r.bottom - t - h - 2 * margin;
  MoveWindow(hEditor, l, t, w, h, TRUE);
  // setter
  w = EDITOR_WIDTH - BUTTON_WIDTH * 2 - margin * 2, h = BUTTON_HEIGHT, t = r.bottom - h - margin;
  MoveWindow(hEditSize, l, t, w, h, TRUE);
  // button size
  w = BUTTON_WIDTH;
  l = r.right - w * 2 - margin * 2;
  MoveWindow(hBtnSize, l, t, w, h, TRUE);
  // button next
  w = BUTTON_WIDTH;
  l = r.right - w - margin;
  MoveWindow(hBtnNext, l, t, w, h, TRUE);
  // canvas
  l = r.left + margin, t = r.top + margin;
  w = r.right - EDITOR_WIDTH - 2 * margin - l, h = r.bottom - t - margin;
  MoveWindow(hCanvas, l, t, w, h, TRUE);
}

void RegisterCanvas() {
  WNDCLASS wnd = {0};
  wnd.style = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW;
  wnd.lpfnWndProc = CanvasProc;
  wnd.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wnd.lpszClassName = "canvas";
  RegisterClass(&wnd);
}

LRESULT CALLBACK EditorProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  if (msg == WM_KEYDOWN && wp == VK_RETURN && GetKeyState(VK_CONTROL) < 0) {
    HandleOK();
    return 0;
  }
  if (msg == WM_CHAR && wp == '\n' && GetKeyState(VK_CONTROL) < 0) {
    return 0;
  }
  return CallWindowProc(editorProc, hwnd, msg, wp, lp);
}

LRESULT CALLBACK SetterProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  if (msg == WM_KEYDOWN && wp == VK_RETURN) {
    HandleSize();
    return 0;
  }
  return CallWindowProc(setterProc, hwnd, msg, wp, lp);
}

void ScrollCanvas(int d) {
  ScrollWindow(hCanvas, 0, d, nullptr, nullptr);
  UpdateWindow(hCanvas);
}

LRESULT CALLBACK CanvasProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  HDC hdc;
  PAINTSTRUCT ps;

  switch (message) {
    case WM_PAINT: {
      hdc = BeginPaint(hWnd, &ps);
      RenderFormula(hWnd, hdc);
      EndPaint(hWnd, &ps);
      break;
    }
    case WM_ERASEBKGND: {
      hdc = (HDC)wParam;
      RECT r;
      GetClientRect(hWnd, &r);
      FillRect(hdc, &r, (HBRUSH)GetStockObject(WHITE_BRUSH));
      break;
    }
    default: return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}
