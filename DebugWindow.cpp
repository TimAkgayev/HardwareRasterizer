#include "DebugWindow.h"

DebugWindow::DebugWindow(HINSTANCE hInstance)
{
	WNDCLASSEX winclass;

	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = DebugWindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hInstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = NULL;
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = TEXT(WINDOW_CLASS_NAME);
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&winclass);
	
	mWindowHandle = CreateWindowEx(NULL,
	TEXT(WINDOW_CLASS_NAME),     // class
	TEXT("Debug"), // title
	WS_VISIBLE | WS_SYSMENU | WS_THICKFRAME | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,		//style
	0, 0,	  // initial x,y
	700, 300,  // initial width, height
	NULL,		// handle to parent 
	NULL,		 // handle to menu
	hInstance,	// instance of this application
	NULL);	// extra creation parms

	
	softwareRasterizer = new SoftwareRasterizer::SRasterizer(mWindowHandle);

}

DebugWindow::~DebugWindow()
{
}

void DebugWindow::DisplayBitmap(SoftwareBitmap::Bitmap * bmp)
{

	softwareRasterizer->DrawBitmapWithClipping(bmp, 0, 0);
	
}


LRESULT CALLBACK DebugWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	// process any messages that we didn't take care of
	return (DefWindowProc(hwnd, msg, wParam, lParam));
}
