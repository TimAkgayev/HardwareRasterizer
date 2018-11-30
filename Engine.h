#pragma once
#include "Application.h"

class Engine
{
public:
	void Initialization(Application* AppInstance);
	int Loop();
	void Shutdown();

	void CreateEngineWindow(const wchar_t* WindowClassName, HINSTANCE hInstance);

protected:
	Application* mApplicationInstance;
	HWND mMainWindowHandle;
};