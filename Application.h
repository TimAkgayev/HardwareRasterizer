#pragma once
#include <Windows.h>

enum { UPDATE_NORMAL = 0, UPDATE_RESET };

class Application
{
public: 

	Application() {}
	virtual ~Application() {}

	virtual void ApplicationInitialization() = 0;
	virtual void ApplicationRender() = 0;
	virtual int  ApplicationUpdate() = 0;
	virtual void ApplicationShutdown() = 0;
	virtual void ApplicationWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;

	wchar_t* GetWindowTitle();
	int GetClientWidth();
	int GetClientHeight();
	
	
protected:
	int mClientWidth;
	int mClientHeight;
	wchar_t* mWindowTitle;
	

};