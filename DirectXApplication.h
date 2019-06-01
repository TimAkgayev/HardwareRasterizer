#pragma once
#include <windows.h>
#include "d3dcommon.h"
#include "GameTimer.h"
#include <string>
#include <vector>

using namespace DirectX;


class DirectXApplication
{
public:
	DirectXApplication(HINSTANCE hInstance);
	virtual ~DirectXApplication();

	virtual void initApp();
	virtual void updateScene(float dt);
	virtual void drawScene();

	virtual LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam);

	int run();

protected:
	void initMainWindow();
	void initDirectX();

protected:


	HINSTANCE mhAppInst;
	HWND      mhMainWnd;
	bool      mAppPaused;

	int mClientWidth;
	int mClientHeight;

	GameTimer mTimer;

	std::wstring mMainWndCaption;


	ID3D10Device*		 mD3D10Device;
	ID3D10Texture2D*     mD3D10DepthStencilTexture;
	ID3D10DepthStencilView* mD3D10DepthStencilView;
	IDXGISwapChain*		 mD3D10SwapChain;
	ID3D10RasterizerState* mRasterizerStateSolid;
	
	
	ID3D10BlendState*    mD3D10BlendState;
	
	ID3D10SamplerState*  mD3D10SamplerState;

	ID3D10RenderTargetView*     mD3D10RenderTargetView;
	D3D10_VIEWPORT				mD3D10Viewport;


};