#pragma once
#include <Windows.h>
#include <malloc.h>
#include "RasterizerInterface.h"

enum { UPDATE_NORMAL = 0, UPDATE_RESET };

__declspec(align(16)) class Application
{
public: 

	Application() {}
	virtual ~Application() {}

	virtual void ApplicationInitialization(HWND window) = 0;
	virtual int  ApplicationUpdate() = 0;
	virtual void ApplicationShutdown() = 0;
	virtual void ApplicationWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;

	wchar_t* GetWindowTitle();
	int GetClientWidth();
	int GetClientHeight();

	//making sure we're hitting those 16 bit boundaries correctly for directx math
	void* operator new(size_t i);
	void operator delete(void* p);
	
	void SetRasterizer(RasterizerInterface* rasterizer);

protected:
	int mClientWidth;
	int mClientHeight;
	wchar_t* mWindowTitle;
	RasterizerInterface* mRasterizer;
	HWND mWindow;
	

};