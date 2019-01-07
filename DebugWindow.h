#pragma once
#include "Bitmap.h"
#include <Windows.h>
#include "C:\\Users\Tim\Documents\Visual Studio Projects\SoftwareRasterizer\SoftwareRasterizer\SRasterizer.h"



#define WINDOW_CLASS_NAME "DebugWindowClass"

class DebugWindow
{
public:
	DebugWindow(HINSTANCE hInstance);
	~DebugWindow();

	void DisplayBitmap(SoftwareBitmap::Bitmap* bmp);

protected:
	HWND mWindowHandle;
	SoftwareRasterizer::SRasterizer* softwareRasterizer;
};

LRESULT CALLBACK DebugWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
