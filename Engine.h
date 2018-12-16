#pragma once
#include "Application.h"
#include <d3d10.h>
#include "D3DX\D3DX10.h"
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "Camera.h"
#include "WorldObject.h"
#include "Mesh.h"
#include "RasterizerInterface.h"

#define NUM_VERTEX_ELEMENTS 2
using namespace DirectX;

struct D3DVertex
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
};

struct MeshDescriptor
{
	ID3D10Buffer*    VertexBuffer;
	ID3D10Buffer*    IndexBuffer;
	XMMATRIX         WorldMatrix;
	WorldObject*     MeshObjectPtr;
};

class Engine : public RasterizerInterface
{
public:
	void Initialization(Application* AppInstance);
	int Loop();
	void Shutdown();
	void CreateEngineWindow(const wchar_t* WindowClassName, HINSTANCE hInstance);


	//RasterizerInterface
	void SetViewMatrix(XMMATRIX& view) override;
	void DrawWorldObject(WorldObject* obj, XMMATRIX& worldMatrix) override;

protected:

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	

	Application* mApplicationInstance;
	HWND mMainWindowHandle;

	ID3D10Device*		mD3D10Device;
	IDXGISwapChain*		mD3D10SwapChain;
	ID3D10VertexShader* mD3D10VertexShader;
	ID3D10PixelShader*  mD3D10PixelShader;
	ID3D10InputLayout*  mD3D10InputLayout;
	ID3D10BlendState*   mD3D10BlendState;
	ID3D10Buffer*       mD3D10ConstantBuffer;

	ID3D10RenderTargetView*     mD3D10RenderTargetView;
	D3D10_VIEWPORT				mD3D10Viewport;

	XMMATRIX mViewMatrix;
	XMMATRIX mProjectionMatrix;

	std::vector<MeshDescriptor> mLoadedMeshes;
	std::vector<UINT>  mMeshDrawList;




	


}; 

