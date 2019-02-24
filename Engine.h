#pragma once
#include "Interface_Application.h"
#include <d3d10.h>
#include "D3DX\D3DX10.h"
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "Camera.h"
#include "WorldObject.h"
#include "Mesh.h"
#include "RasterizerInterface.h"
#include "Interface_AutoUpdate.h"


#define NUM_MESH_VERTEX_ELEMENTS 2
#define NUM_LINE_VERTEX_ELEMENTS 2

using namespace DirectX;

struct D3DMeshVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

struct D3DLineVertex
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
	ID3D10ShaderResourceView* mTextureResourceView;
};

struct LineDescriptor
{
	ID3D10Buffer*    VertexBuffer;
	XMMATRIX         WorldMatrix;
	WorldObject*     LineObjectPtr;
};

class Engine : public RasterizerInterface
{
public:
	int Loop();
	void Shutdown();
	void CreateEngineWindow(const wchar_t* WindowClassName, HINSTANCE hInstance, Application* appInstance);


	//RasterizerInterface
	void SetViewMatrix(const XMMATRIX& view) override;
	void DrawWorldObject(WorldObject* obj, XMMATRIX& worldMatrix) override;

protected:

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	

	Application* mApplicationInstance;
	HWND mMainWindowHandle;

	ID3D10Device*		 mD3D10Device;
	ID3D10Texture2D* mD3D10DepthStencilTexture;
	ID3D10DepthStencilView* mD3D10DepthStencilView;
	IDXGISwapChain*		 mD3D10SwapChain;
	ID3D10VertexShader*  mMeshVertexShader;
	ID3D10PixelShader*   mMeshPixelShader;
	ID3D10VertexShader*  mLineVertexShader;
	ID3D10PixelShader*   mLinePixelShader;
	ID3D10InputLayout*   mD3D10MeshInputLayout;
	ID3D10InputLayout*   mD3D10LineInputLayout;
	ID3D10BlendState*    mD3D10BlendState;
	ID3D10Buffer*        mD3D10ConstantBuffer;
	ID3D10SamplerState*  mD3D10SamplerState;

	ID3D10RenderTargetView*     mD3D10RenderTargetView;
	D3D10_VIEWPORT				mD3D10Viewport;

	XMMATRIX mViewMatrix;
	XMMATRIX mNewViewMatrix;
	XMMATRIX mProjectionMatrix;

	std::vector<MeshDescriptor> mLoadedMeshes;
	std::vector<LineDescriptor> mLoadedLines;

	std::vector<UINT>  mMeshDrawList;
	std::vector<UINT>  mLineDrawList;






}; 

