#pragma once
#include "ApplicationInterface.h"
#include "Camera.h"
#include "Mesh.h"
#include "DebugWindow.h"
#include "C:\\Users\Tim\Documents\Visual Studio Projects\SoftwareRasterizer\SoftwareBitmap\Bitmap.h"
#include <map>
#include "cam.h"

enum { W_TIMER = 1, A_TIMER, D_TIMER, S_TIMER, UP_TIMER, DOWN_TIMER, LEFT_TIMER, RIGHT_TIMER };

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

		mCubeMesh.SetTexture(L"..\\HardwareRasterizer\\Textures\\Brick.bmp");
		mFloorMesh.SetTexture(L"..\\HardwareRasterizer\\Textures\\Test.bmp");

		mUp.CreateLine(0, 0, 0, 0, 1, 0, XMFLOAT4(0, 0, 1.0f, 1.0f), XMFLOAT4(0, 0, 1.0f, 1.0f));
		mRight.CreateLine(0, 0, 0, 1, 0, 0, XMFLOAT4(1.0f, 0, 0.0f, 1.0f), XMFLOAT4(1.0f, 0, 0.0f, 1.0f));
		mLookAt.CreateLine(0, 0, 0, 0, 0, 1, XMFLOAT4(0, 1.0f, 0.0f, 1.0f), XMFLOAT4(0, 1.0f, 0.0f, 1.0f));


		topMark.CreateDummyCube(20, 20);
		backMark.CreateDummyCube(20, 20);
		bottomMark.CreateDummyCube(20, 20);


		topMark.SetTexture(L"..\\HardwareRasterizer\\Textures\\TopBrick.bmp");
		backMark.SetTexture(L"..\\HardwareRasterizer\\Textures\\BackBrick.bmp");
		bottomMark.SetTexture(L"..\\HardwareRasterizer\\Textures\\BottomBrick.bmp");


		mHINSTANCE = hInstance;

		mBitmap = new SoftwareBitmap::Bitmap(L"..\\HardwareRasterizer\\Heightmaps\\TestFloor.bmp");
	}
	virtual int  ApplicationUpdate() 
	{
		// rotate a cube
		static float t = 0.0f;

		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();
		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;
		t = (dwTimeCur - dwTimeStart) / 1000.0f;
		XMMATRIX mat = XMMatrixRotationX(t);
		mRasterizer->DrawWorldObject((WorldObject*)&mCubeMesh, mat);

		//update the locator
		XMFLOAT3 upV = mcam.GetUpVector();
		XMFLOAT3 rightV = mcam.GetRightVector();
		XMFLOAT3 lookV = mcam.GetLookAtVector();

		mUp.SetSecondPoint(upV.x, upV.y, upV.z);
		mRight.SetSecondPoint(rightV.x, rightV.y, rightV.z);
		mLookAt.SetSecondPoint(lookV.x, lookV.y, lookV.z);

		XMMATRIX upMat, rightMat, lookMat;
		upMat = XMMatrixIdentity();
		rightMat = XMMatrixIdentity();
		lookMat = XMMatrixIdentity();

		upMat = rightMat = lookMat = XMMatrixScaling( 3, 3 , 3);

		XMMATRIX viewOffset = XMMatrixTranslation(-23.0f, -13.0f, 40.0f);

		upMat *= viewOffset;
		rightMat *= viewOffset;
		lookMat *= viewOffset;

		XMMATRIX viewR = mcam.GetViewRotationMatrix();
		XMMATRIX viewRInv = XMMatrixInverse(NULL, viewR);

		upMat *= viewRInv;
		rightMat *= viewRInv;
		lookMat *= viewRInv;


		XMMATRIX viewT = mcam.GetViewTranslationMatrix();
		XMMATRIX viewTInv = XMMatrixInverse(NULL, viewT);

		upMat *= viewTInv;
		rightMat *= viewTInv;
		lookMat *= viewTInv;

		
		

		mRasterizer->DrawWorldObject((WorldObject*)&mUp, upMat);
		mRasterizer->DrawWorldObject((WorldObject*)&mRight, rightMat);
		mRasterizer->DrawWorldObject((WorldObject*)&mLookAt, lookMat);


		XMMATRIX topCubeMat, bottomCubeMat, backCubeMat;
		topCubeMat = XMMatrixTranslation(0, 400.0f, -1000.0f);
		bottomCubeMat = XMMatrixTranslation(0, -400.0f, -1000.0f);
		backCubeMat = XMMatrixTranslation(0, 0.0f, -2000.0f);


		mRasterizer->DrawWorldObject((WorldObject*)&topMark, topCubeMat);
		mRasterizer->DrawWorldObject((WorldObject*)&bottomMark, bottomCubeMat);
		mRasterizer->DrawWorldObject((WorldObject*)&backMark, backCubeMat);


		XMMATRIX floorMat = XMMatrixIdentity();
		mRasterizer->DrawWorldObject((WorldObject*)&mFloorMesh, floorMat);

		//mRasterizer->SetViewMatrix(mCamera.View());
		mRasterizer->SetViewMatrix(mcam.GetViewMatrix());

		float moveSpeed = 0.5f;

		static DWORD timeDeltaUp = 2;
		static DWORD timeDeltaDown = 2;
		static DWORD timeDeltaLeft = 2;
		static DWORD timeDeltaRight = 2;

		if (GetAsyncKeyState(0x57)) //W key
			mcam.MoveAlongLookAt(-moveSpeed);

		if (GetAsyncKeyState(0x53)) //S key
			mcam.MoveAlongLookAt(moveSpeed);

		if (GetAsyncKeyState(0x41)) //A key
			mcam.MoveAlongRight(-moveSpeed);

		if (GetAsyncKeyState(0x44)) //D key
			mcam.MoveAlongRight(moveSpeed);

		XMMATRIX yawMatrix, pitchMatrix;

		if (GetAsyncKeyState(VK_UP) && ((timeGetTime() - timeDeltaUp) > 10))
		{
			timeDeltaUp = timeGetTime();
			pitchMatrix = mcam.Pitch(-1);
		}

		if (GetAsyncKeyState(VK_DOWN) && ((timeGetTime() - timeDeltaDown) > 10))
		{
			timeDeltaDown = timeGetTime();
			pitchMatrix = mcam.Pitch(1);
		}

		if (GetAsyncKeyState(VK_LEFT) && ((timeGetTime() - timeDeltaLeft) > 10))
		{
			timeDeltaLeft = timeGetTime();
			yawMatrix = mcam.Yaw(-1);
		}

		if (GetAsyncKeyState(VK_RIGHT) && ((timeGetTime() - timeDeltaRight) > 10))
		{
			timeDeltaRight = timeGetTime();
			yawMatrix = mcam.Yaw(1);
		}


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

		

		}break;

		case WM_KEYDOWN: 
		{
	


		} break;

		case WM_KEYUP:
		{
		
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
	Line mUp;
	Line mRight;
	Line mLookAt;

	Mesh topMark, bottomMark, backMark;

	cam mcam;

	SoftwareBitmap::Bitmap* mBitmap;

	HINSTANCE mHINSTANCE;
	
};