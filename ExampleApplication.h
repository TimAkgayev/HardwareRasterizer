#pragma once
#include "ApplicationInterface.h"
#include "Camera.h"
#include "Mesh.h"
#include "DebugWindow.h"
#include "C:\\Users\Tim\Documents\Visual Studio Projects\SoftwareRasterizer\SoftwareBitmap\Bitmap.h"
#include <map>

enum { W_TIMER = 1, A_TIMER, D_TIMER, S_TIMER, UP_TIMER, DOWN_TIMER };

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
		mKeyDownMap.insert(std::make_pair(VK_UP, false));
		mKeyDownMap.insert(std::make_pair(VK_DOWN, false));

		mDbgWindow = new DebugWindow(mHINSTANCE);
	}


	virtual void ApplicationInitialization(HWND window, HINSTANCE hInstance) 
	{
		mWindow = window;
		mCubeMesh.CreateDummyCube(10, 20);
		mFloorMesh.CreateFloor(L"..\\HardwareRasterizer\\Heightmaps\\TestFloor.bmp", 10, 10);
		mHINSTANCE = hInstance;

		mBitmap = new SoftwareBitmap::Bitmap(L"..\\HardwareRasterizer\\Heightmaps\\TestFloor.bmp");
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

		XMMATRIX floorMat = XMMatrixIdentity();
		mRasterizer->DrawWorldObject((WorldObject*)&mFloorMesh, floorMat);

		mRasterizer->SetViewMatrix(mCamera.View());

		mDbgWindow->DisplayBitmap(mBitmap);

		
		return UPDATE_NORMAL; 
	}
	virtual void ApplicationShutdown() {};
	virtual void ApplicationWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{

		switch (msg)
		{


		case WM_TIMER:
		{

			float moveSpeed = 5.0f;

			if (wParam == W_TIMER)
				mCamera.Move(XMFLOAT3(0.0f, 0.0f, moveSpeed));
			else if (wParam == S_TIMER)
				mCamera.Move(XMFLOAT3(0.0f, 0.0f, -moveSpeed));
			else if (wParam == D_TIMER)
				mCamera.Move(XMFLOAT3(moveSpeed, 0.0f, 0.0f));
			else if (wParam == A_TIMER)
				mCamera.Move(XMFLOAT3(-moveSpeed, 0.0f, 0.0f));
			else if (wParam == UP_TIMER)
				mCamera.Move(XMFLOAT3(0.0f, moveSpeed, 0.0f));
			else if (wParam == DOWN_TIMER)
				mCamera.Move(XMFLOAT3(0.0f, -moveSpeed, 0.0f));
		

		}break;

		case WM_KEYDOWN: 
		{
	
			if (wParam == 0x57) //W key
				if (!mKeyDownMap[wParam])
				{
					SetTimer(mWindow, W_TIMER, 10, NULL);
					mKeyDownMap[wParam] = true;
				}
			
			 if (wParam == 0x53) //S key
				if (!mKeyDownMap[wParam])
				{
					SetTimer(mWindow, S_TIMER, 10, NULL);
					mKeyDownMap[wParam] = true;
				}
			 if (wParam == 0x41) //A key
				if (!mKeyDownMap[wParam])
				{
					SetTimer(mWindow, A_TIMER, 10, NULL);
					mKeyDownMap[wParam] = true;
				}
			 if (wParam == 0x44) //D key
				if (!mKeyDownMap[wParam])
				{
					SetTimer(mWindow, D_TIMER, 10, NULL);
					mKeyDownMap[wParam] = true;
				}
			 if (wParam == VK_UP)
				if (!mKeyDownMap[wParam])
				{
					SetTimer(mWindow, UP_TIMER, 10, NULL);
					mKeyDownMap[wParam] = true;
				}
			 if (wParam == VK_DOWN) 
				if (!mKeyDownMap[wParam])
				{
					SetTimer(mWindow, DOWN_TIMER, 10, NULL);
					mKeyDownMap[wParam] = true;
				}


		} break;

		case WM_KEYUP:
		{
			if (wParam == 0x57) //W key
			{
				KillTimer(mWindow, W_TIMER);
				mKeyDownMap[wParam] = false;
		
			}
			 if (wParam == 0x53) //S key
			{
				KillTimer(mWindow, S_TIMER);
				mKeyDownMap[wParam] = false;
			}
			 if (wParam == 0x41) //A key
			{
				KillTimer(mWindow, A_TIMER);
				mKeyDownMap[wParam] = false;
			}
			 if (wParam == 0x44) //D key
			{
				KillTimer(mWindow, D_TIMER);
				mKeyDownMap[wParam] = false;
			}
			 if (wParam == VK_UP) //D key
			{
				KillTimer(mWindow, UP_TIMER);
				mKeyDownMap[wParam] = false;
			}
			 if (wParam == VK_DOWN) //D key
			{
				KillTimer(mWindow, DOWN_TIMER);
				mKeyDownMap[wParam] = false;
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

	DebugWindow* mDbgWindow;
	Camera mCamera;
	Mesh mCubeMesh;
	Mesh mFloorMesh;

	SoftwareBitmap::Bitmap* mBitmap;

	HINSTANCE mHINSTANCE;
	
};