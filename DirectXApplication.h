#pragma once

#include "common_includes.h"
#include "GameTimer.h"


using namespace DirectX;


class DirectXApplication
{
public:
	DirectXApplication(HINSTANCE hInstance);
	DirectXApplication(const DirectXApplication& rhs) = delete;
	DirectXApplication& operator=(const DirectXApplication& rhs) = delete;
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


	ID3D11Device*		 mD3DDevice;
	ID3D11Texture2D*     mD3D11DepthStencilTexture;
	ID3D11DepthStencilView* mD3D11DepthStencilView;
	IDXGISwapChain*		 mD3D11SwapChain;
	ID3D11RasterizerState* mRasterizerStateSolid;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11DepthStencilState * mDepthStencilState;

	ID3D11BlendState*    mD3D11BlendState;
	
	ID3D11SamplerState*  mD3D11SamplerState;

	ID3D11RenderTargetView*     mD3D11RenderTargetView;
	D3D11_VIEWPORT				mD3D11Viewport;


};