#pragma once
#include "Application.h"

class ExampleApp : public Application
{
public:
	ExampleApp() {
		mClientWidth = 800;
		mClientHeight = 500;
		mWindowTitle = TEXT("Example Application");
	}

	virtual void ApplicationInitialization() {}
	virtual int  ApplicationUpdate() { return UPDATE_NORMAL; }
	virtual void ApplicationShutdown() {};
	virtual void ApplicationWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{

		switch (msg)
		{
		case WM_DESTROY: //window is destoyed
		{
			PostQuitMessage(0);

		} break;

		default: break;
		};
	}
};