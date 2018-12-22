#pragma once
#include "Application.h"
#include "Camera.h"
#include "Mesh.h"
#include <map>

enum { W_TIMER = 1, A_TIMER, D_TIMER, S_TIMER };

class ExampleApp : public Application
{
public:
	ExampleApp() {
		mClientWidth = 800;
		mClientHeight = 500;
		mWindowTitle = TEXT("Example Application");

		//set the default control keys to up position
		mKeyDownMap.insert(std::make_pair(0x57, false));
		mKeyDownMap.insert(std::make_pair(0x53, false));
		mKeyDownMap.insert(std::make_pair(0x41, false));
		mKeyDownMap.insert(std::make_pair(0x44, false));
	}


	virtual void ApplicationInitialization(HWND window) 
	{
		mWindow = window;
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

		mRasterizer->SetViewMatrix(mCamera.View());

		
		return UPDATE_NORMAL; 
	}
	virtual void ApplicationShutdown() {};
	virtual void ApplicationWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{

		switch (msg)
		{


		case WM_TIMER:
		{
			if (wParam == W_TIMER)
				mCamera.Move(XMFLOAT3(0.0f, 0.0f, 1.0f));
			else if (wParam == S_TIMER)
				mCamera.Move(XMFLOAT3(0.0f, 0.0f, -1.0f));
			else if (wParam == D_TIMER)
				mCamera.Move(XMFLOAT3(1.0f, 0.0f, 0.0f));
			else if (wParam == A_TIMER)
				mCamera.Move(XMFLOAT3(-1.0f, 0.0f, 0.0f));	
		

		}break;

		case WM_KEYDOWN: 
		{
	
			if (wParam == 0x57) //W key
				if (!mKeyDownMap[0x57])
				{
					SetTimer(mWindow, W_TIMER, 10, NULL);
					mKeyDownMap[0x57] = true;
				}
			
			if (wParam == 0x53) //S key
				if (!mKeyDownMap[0x53])
				{
					SetTimer(mWindow, S_TIMER, 10, NULL);
					mKeyDownMap[0x53] = true;
				}
			if (wParam == 0x41) //A key
				if (!mKeyDownMap[0x41])
				{
					SetTimer(mWindow, A_TIMER, 10, NULL);
					mKeyDownMap[0x41] = true;
				}
			if (wParam == 0x44) //D key
				if (!mKeyDownMap[0x44])
				{
					SetTimer(mWindow, D_TIMER, 10, NULL);
					mKeyDownMap[0x44] = true;
				}

		} break;

		case WM_KEYUP:
		{
			if (wParam == 0x57) //W key
			{
				KillTimer(mWindow, W_TIMER);
				mKeyDownMap[0x57] = false;
		
			}
			if (wParam == 0x53) //S key
			{
				KillTimer(mWindow, S_TIMER);
				mKeyDownMap[0x53] = false;
			}
			if (wParam == 0x41) //A key
			{
				KillTimer(mWindow, A_TIMER);
				mKeyDownMap[0x41] = false;
			}
			if (wParam == 0x44) //D key
			{
				KillTimer(mWindow, D_TIMER);
				mKeyDownMap[0x44] = false;
			}

		}break;

		case WM_DESTROY: //window is destoyed
		{
			PostQuitMessage(0);

		} break;

		default: break;
		};
	}

	

private:

	std::map<int, bool> mKeyDownMap;


	Camera mCamera;
	Mesh mCubeMesh;
	
};