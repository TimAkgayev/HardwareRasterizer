#include "Engine.h"

void Engine::Initialization(Application* AppInstance)
{
	mApplicationInstance = AppInstance;
	mApplicationInstance->ApplicationInitialization();

}

int Engine::Loop()
{
	//Game update
	if (mApplicationInstance->ApplicationUpdate() == UPDATE_RESET)
	{
		

		//release window
		DestroyWindow(mMainWindowHandle);


		return UPDATE_RESET;
	}

	return UPDATE_NORMAL;
}

void Engine::Shutdown()
{

}

void Engine::CreateEngineWindow(const wchar_t* WindowClassName, HINSTANCE hInstance)
{
	HWND hwnd;
	POINT windowFrameDim = { 16, 39 };



	if (!(hwnd = CreateWindowEx(NULL,
		WindowClassName,     // class
		mApplicationInstance->GetWindowTitle(), // title
		WS_VISIBLE | WS_SYSMENU | WS_THICKFRAME | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,		//style
		0, 0,	  // initial x,y
		mApplicationInstance->GetClientWidth() + windowFrameDim.x, mApplicationInstance->GetClientHeight() + windowFrameDim.y,  // initial width, height
		NULL,		// handle to parent 
		NULL,		 // handle to menu
		hInstance,	// instance of this application
		NULL)))	// extra creation parms 
	{
	}
	


	// save main window handle
	mMainWindowHandle = hwnd;

}