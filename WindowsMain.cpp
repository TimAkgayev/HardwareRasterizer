#include <Windows.h>
#include "ExampleApplication.h"
#include "Engine.h"


Application* RunningApplication;
Engine ApplicationEngine;

#define APPLICATION_WINDOW_CLASS_NAME "HR_APPLICATION"

void DefineApplication() {

	//create an application
	RunningApplication = new ExampleApp();
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RunningApplication->ApplicationWindowProc(hwnd, msg, wParam, lParam);

	return DefWindowProc(hwnd, msg, wParam, lParam);
}



int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpCmdLine, int nCmdShow)
{

	DefineApplication();
	
	//lock thread to one processor to make the timing work
	DWORD_PTR mask = 1;
	SetThreadAffinityMask(GetCurrentThread(), mask);


	WNDCLASSEX winclass;
	MSG		   msg;

	// setup the default window class
	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = NULL;
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = TEXT(APPLICATION_WINDOW_CLASS_NAME);
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&winclass))
		return(0);



//entry point when resetting
resetEntry:

	ApplicationEngine.Initialization(RunningApplication);

	ApplicationEngine.CreateEngineWindow(TEXT(APPLICATION_WINDOW_CLASS_NAME), hinstance);



	while (TRUE)
	{
		// test if there is a message in queue, if so get it and remove it from the queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{

			if (msg.message == WM_QUIT)
				break;


			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else
		{

			int code = ApplicationEngine.Loop();
			if (code == UPDATE_RESET)
			{
				ApplicationEngine.Shutdown();
				goto resetEntry;
			}
		}

	}

	ApplicationEngine.Shutdown();

	return msg.wParam;

}

