#include "config.h"

#if defined(BUILD_WIN32) && !defined(MEM_CHECK)

#include "latex.h"
#include "platform/gdi_win/graphic_win32.h"
#include "samples.h"

#include <time.h>
#include <cstdlib>
#include <iostream>

#include <gdiplus.h>
#include <objidl.h>
#include <tchar.h>
#include <windows.h>

#define ID_SETTER 256
#define ID_CANVAS 512
#define ID_EDITBOX 1024
#define ID_BUTTON_SIZE 2048
#define ID_BUTTON_RANDOM 4096

#define BUTTON_WIDTH 140
#define BUTTON_HEIGHT 35
#define EDITOR_WIDTH 480

using namespace std;
using namespace tex;
using namespace Gdiplus;

TeXRender* _render = nullptr;
int _size = 26;
color _color = 0xff424242;

HWND hEditor, hBtnSize, hBtnRandom, hCanvas, hEditSize;
WNDPROC editorProc, setterProc;

void RegisterCanvas();

void CreateCtrl(HINSTANCE, HWND);

void ResizeCtrl(HWND);

void HandleOK();

void HandleRandom();

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
  GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
  init();

  RegisterCanvas();

  wndClass.style = CS_HREDRAW | CS_VREDRAW;
  wndClass.lpfnWndProc = WndProc;
  wndClass.cbClsExtra = 0;
  wndClass.cbWndExtra = 0;
  wndClass.hInstance = hInstance;
  wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wndClass.lpszMenuName = NULL;
  wndClass.lpszClassName = TEXT("LaTeX");

  RegisterClass(&wndClass);

  hWnd = CreateWindow(
      TEXT("LaTeX"),        // window class name
      TEXT("LaTeX"),        // window caption
      WS_OVERLAPPEDWINDOW,  // window style
      0,                    // initial x position
      0,                    // initial y position
      780,                  // initial x size
      480,                  // initial y size
      NULL,                 // parent window handle
      NULL,                 // window menu handle
      hInstance,            // program instance handle
      NULL);                // creation parameters

  CreateCtrl(hInstance, hWnd);
  ShowWindow(hWnd, SW_SHOWMAXIMIZED);
  UpdateWindow(hWnd);

  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  delete _render;
  LaTeX::release();
  GdiplusShutdown(gdiplusToken);
  return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    case WM_COMMAND:
      if (wParam == ID_BUTTON_SIZE) {
        HandleSize();
      } else if (wParam == ID_BUTTON_RANDOM) {
        HandleRandom();
      }
      return 0;
    case WM_SIZE:
      ResizeCtrl(hWnd);
      return 0;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
}  // WndProc

void init() {
  LaTeX::init();
  _render = LaTeX::parse(L"\\text{What a beautiful day}", 720, _size, _size / 3.f, _color);
}

void HandleRandom() {
  srand(time(NULL));
  int idx = rand() % tex::SAMPLES_COUNT;
  if (_render != nullptr) {
    delete _render;
  }
  RECT r;
  GetClientRect(hCanvas, &r);
  _render = LaTeX::parse(wstring(tex::SAMPLES[idx]), r.right - r.left, _size, _size / 3.f, _color);
  InvalidateRect(hCanvas, NULL, TRUE);
  UpdateWindow(hCanvas);
}

void HandleOK() {
  int len = GetWindowTextLengthW(hEditor);
  wchar_t* txt = new wchar_t[len + 10];
  GetWindowTextW(hEditor, txt, len + 10);
  if (_render != nullptr) {
    delete _render;
  }
  RECT r;
  GetClientRect(hCanvas, &r);
  _render = LaTeX::parse(wstring(txt), r.right - r.left, _size, _size / 3.f, _color);
  InvalidateRect(hCanvas, NULL, TRUE);
  UpdateWindow(hCanvas);
  delete[] txt;
}

void HandleSize() {
  if (_render == nullptr)
    return;
  int len = GetWindowTextLength(hEditSize);
  if (len == 0)
    return;
  char* txt = new char[len + 10];
  GetWindowText(hEditSize, txt, len + 10);
  string x = txt;
  valueof(x, _size);
  _render->setTextSize(_size);
  InvalidateRect(hCanvas, NULL, TRUE);
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
      NULL,
      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
      l,
      t,
      w,
      h,
      hwnd,
      (HMENU)ID_EDITBOX,
      hInst,
      NULL);
  HFONT hf = CreateFont(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Consolas");
  SendMessage(hEditor, WM_SETFONT, (WPARAM)hf, 0);
  SetFocus(hEditor);
  editorProc = (WNDPROC)SetWindowLongPtr(hEditor, GWLP_WNDPROC, (LONG_PTR)EditorProc);
  // size setter
  hEditSize = CreateWindowEx(
      WS_EX_TOPMOST,
      "edit",
      NULL,
      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_NUMBER,
      l,
      t,
      w,
      h,
      hwnd,
      (HMENU)ID_SETTER,
      hInst,
      NULL);
  SendMessage(hEditSize, WM_SETFONT, (WPARAM)hf, 0);
  setterProc = (WNDPROC)SetWindowLongPtr(hEditSize, GWLP_WNDPROC, (LONG_PTR)SetterProc);
  // button size
  hBtnSize = CreateWindowEx(
      WS_EX_TOPMOST,
      "Button",
      NULL,
      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
      l,
      t,
      w,
      h,
      hwnd,
      (HMENU)ID_BUTTON_SIZE,
      hInst,
      NULL);
  SendMessage(hBtnSize, WM_SETFONT, (WPARAM)hf, 0);
  SendMessage(hBtnSize, WM_SETTEXT, (WPARAM)NULL, (LPARAM)("Set Text Size"));
  // button random
  hBtnRandom = CreateWindowEx(
      WS_EX_TOPMOST,
      "Button",
      NULL,
      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
      l,
      t,
      w,
      h,
      hwnd,
      (HMENU)ID_BUTTON_RANDOM,
      hInst,
      NULL);
  SendMessage(hBtnRandom, WM_SETFONT, (WPARAM)hf, 0);
  SendMessage(hBtnRandom, WM_SETTEXT, (WPARAM)NULL, (LPARAM)("Random Example"));
  // canvas
  hCanvas = CreateWindowEx(
      WS_EX_TOPMOST,
      "canvas",
      NULL,
      WS_CHILD | WS_VISIBLE,
      l,
      t,
      w,
      h,
      hwnd,
      (HMENU)ID_CANVAS,
      hInst,
      NULL);
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
  // button random
  w = BUTTON_WIDTH;
  l = r.right - w - margin;
  MoveWindow(hBtnRandom, l, t, w, h, TRUE);
  // canvas
  l = r.left + margin, t = r.top + margin;
  w = r.right - EDITOR_WIDTH - 2 * margin - l, h = r.bottom - t - margin;
  MoveWindow(hCanvas, l, t, w, h, TRUE);
}

void RegisterCanvas() {
  WNDCLASS wnd = {0};
  wnd.style = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW;
  wnd.lpfnWndProc = CanvasProc;
  wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
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
  ScrollWindow(hCanvas, 0, d, NULL, NULL);
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
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

#endif
