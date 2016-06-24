#ifdef _WIN32

#include "latex.h"

#include <iostream>

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <tchar.h>

#include "memcheck.h"

#define ID_SETTER 256
#define ID_CANVAS 512
#define ID_EDITBOX 1024
#define ID_BUTTON 2048
#define EDITOR_WIDTH 480

using namespace std;
using namespace tex;
using namespace tex::core;
using namespace tex::port;
using namespace Gdiplus;

TeXRender* _render = nullptr;
int _size = 26;
color _color = 0xff101010;

HWND hEditor, hBtn, hCanvas, hSetter;
WNDPROC editorProc, setterProc;

void RegisterCanvas();

void CreateCtrl(HINSTANCE, HWND);

void ResizeCtrl(HWND);

void HandleOK();

void HandleSize();

void RenderFormula(HWND, HDC);

void init();

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK CanvasProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK EditorProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK SetterProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow) {
	HWND                hWnd;
	MSG                 msg;
	WNDCLASS            wndClass;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	init();

	RegisterCanvas();

	wndClass.style          = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc    = WndProc;
	wndClass.cbClsExtra     = 0;
	wndClass.cbWndExtra     = 0;
	wndClass.hInstance      = hInstance;
	wndClass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName   = NULL;
	wndClass.lpszClassName  = TEXT("LaTeX");

	RegisterClass(&wndClass);

	hWnd = CreateWindow(
	           TEXT("LaTeX"),   // window class name
	           TEXT("LaTeX"),  // window caption
	           WS_OVERLAPPEDWINDOW,      // window style
	           0,                        // initial x position
	           0,                        // initial y position
	           780,                      // initial x size
	           480,                      // initial y size
	           NULL,                     // parent window handle
	           NULL,                     // window menu handle
	           hInstance,                // program instance handle
	           NULL);                    // creation parameters

	CreateCtrl(hInstance, hWnd);
	ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	UpdateWindow(hWnd);

	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	delete _render;
	LaTeX::release();
	GdiplusShutdown(gdiplusToken);
	return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_COMMAND:
		if (wParam == ID_BUTTON)
			HandleSize();
		return 0;
	case WM_SIZE:
		ResizeCtrl(hWnd);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
} // WndProc

void init() {
	LaTeX::init();
	_render = LaTeX::parse(L"\\text{What a beautiful day}", 720, _size, _size / 3.f, _color);
	DUMP_MEM_STATUS();
}

void HandleOK() {
	int len = GetWindowTextLengthW(hEditor);
	wchar_t* txt = new wchar_t[len + 10];
	GetWindowTextW(hEditor, txt, len + 10);
	if (_render != nullptr)
		delete _render;
	RECT r;
	GetClientRect(hCanvas, &r);
	_render = LaTeX::parse(wstring(txt), r.right - r.left, _size, _size / 3.f, _color);
	DUMP_MEM_STATUS();
	InvalidateRect(hCanvas, NULL, TRUE);
	UpdateWindow(hCanvas);
	delete[] txt;
}

void HandleSize() {
	if (_render == nullptr)
		return;
	int len = GetWindowTextLength(hSetter);
	if (len == 0)
		return;
	char* txt = new char[len + 10];
	GetWindowText(hSetter, txt, len + 10);
	string x = txt;
	valueof(x, _size);
	_render->setSize(_size);
	InvalidateRect(hCanvas, NULL, TRUE);
	UpdateWindow(hCanvas);
	delete[] txt;
}

void RenderFormula(HWND hwnd, HDC hdc) {
	// draw rectangle
	RECT r;
	GetClientRect(hwnd, &r);
	Graphics g(hdc);
	Graphics2D g2(&g);
	g2.drawRect(r.left, r.top, r.right - r.left - 1, r.bottom - r.top - 1);
	if (_render != nullptr) {
		_render->draw(g2, 10, 10);
	}
}

void CreateCtrl(HINSTANCE hInst, HWND hwnd) {
	const int margin = 10;
	int w = EDITOR_WIDTH, h = 35;
	// edit-box
	RECT r;
	GetClientRect(hwnd, &r);
	int l = r.right - w - margin, t = r.top + margin;
	h = r.bottom - t - h - 2 * margin;
	hEditor = CreateWindowEx(
	              WS_EX_TOPMOST,
	              "edit", NULL,
	              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
	              l, t, w, h,
	              hwnd,
	              (HMENU) ID_EDITBOX,
	              hInst, NULL);
	HFONT hf = CreateFont(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Consolas");
	SendMessage(hEditor, WM_SETFONT, (WPARAM) hf, 0);
	SetFocus(hEditor);
	editorProc = (WNDPROC) SetWindowLong(hEditor, GWL_WNDPROC, (LONG) EditorProc);
	// setter
	w = EDITOR_WIDTH - 70 - margin, h = 35, t = r.bottom - h - margin;
	hSetter = CreateWindowEx(
	              WS_EX_TOPMOST,
	              "edit", NULL,
	              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_NUMBER,
	              l, t, w, h,
	              hwnd,
	              (HMENU) ID_SETTER,
	              hInst, NULL);
	SendMessage(hSetter, WM_SETFONT, (WPARAM) hf, 0);
	setterProc = (WNDPROC) SetWindowLong(hSetter, GWL_WNDPROC, (LONG) SetterProc);
	// button
	w = 70;
	l = r.right - w - margin;
	hBtn = CreateWindowEx(
	           WS_EX_TOPMOST,
	           "Button", NULL,
	           WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	           l, t, w, h,
	           hwnd,
	           (HMENU) ID_BUTTON,
	           hInst,
	           NULL);
	SendMessage(hBtn, WM_SETFONT, (WPARAM) hf, 0);
	SendMessage(hBtn, WM_SETTEXT, (WPARAM) NULL, (LPARAM) ("OK"));
	// canvas
	l = r.left + margin, t = r.top + margin;
	w = r.right - EDITOR_WIDTH - 2 * margin - l, h = r.bottom - t - margin;
	hCanvas = CreateWindowEx(
	              WS_EX_TOPMOST,
	              "canvas", NULL,
	              WS_CHILD | WS_VISIBLE,
	              l, t, w, h,
	              hwnd,
	              (HMENU) ID_CANVAS,
	              hInst,
	              NULL);
}

void ResizeCtrl(HWND hwnd) {
	RECT r;
	GetClientRect(hwnd, &r);
	const int margin = 10;
	// editor
	int w = EDITOR_WIDTH, h = 35;
	int l = r.right - w - margin, t = r.top + margin;
	h = r.bottom - t - h - 2 * margin;
	MoveWindow(hEditor, l, t, w, h, TRUE);
	// setter
	w = EDITOR_WIDTH - 70 - margin, h = 35, t = r.bottom - h - margin;
	MoveWindow(hSetter, l, t, w, h, TRUE);
	// button
	w = 70;
	l = r.right - w - margin;
	MoveWindow(hBtn, l, t, w, h, TRUE);
	// canvas
	l = r.left + margin, t = r.top + margin;
	w = r.right - EDITOR_WIDTH - 2 * margin - l, h = r.bottom - t - margin;
	MoveWindow(hCanvas, l, t, w, h, TRUE);
}

void RegisterCanvas() {
	WNDCLASS wnd = { 0 };
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
	HDC          hdc;
	PAINTSTRUCT  ps;

	switch (message) {
	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);
		RenderFormula(hWnd, hdc);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_ERASEBKGND: {
		hdc = (HDC) wParam;
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
