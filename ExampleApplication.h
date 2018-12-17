#pragma once
#include "Application.h"
#include "Camera.h"
#include "Mesh.h"


class ExampleApp : public Application
{
public:
	ExampleApp() {
		mClientWidth = 800;
		mClientHeight = 500;
		mWindowTitle = TEXT("Example Application");
	}


	virtual void ApplicationInitialization() 
	{
		mCubeMesh.CreateDummyCube(10, 10);
	}
	virtual int  ApplicationUpdate() 
	{
		// Update our time
		static float t = 0.0f;

		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();
		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;
		t = (dwTimeCur - dwTimeStart) / 1000.0f;

		XMMATRIX mat = XMMatrixRotationX(t);
		mRasterizer->DrawWorldObject((WorldObject*)&mCubeMesh, mat);
		
		return UPDATE_NORMAL; 
	}
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

	

private:
	Camera mCamera;
	Mesh mCubeMesh;
	
};