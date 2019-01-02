//Tymur Akgayev
//Demonstration of a DirectDraw based software rasterizer v1

#pragma once
#pragma comment (lib, "winmm.lib") //contains timeGetTime def
#pragma warning(once : 4244)
#define WIN32_LEAN_AND_MEAN  // disables MFC, good for small footprint, bad for debug builds

#include "stdinclude.h"
#include <ddraw.h> // include directdraw
#include <d3d10.h> // and dx10
#include "D3DX10.h"
#include <wrl.h>
#include <DirectXMath.h>

#include "Math.h"
#include "Vertex.h"
#include "Mesh.h"
#include "MacrosAndDefines.h"
#include "ImageManager.h"



using namespace DirectX;
using namespace Microsoft::WRL;

enum DRAW_OPTIONS { DO_POINTLIST = 0, DO_LINELIST, DO_FLAT, DO_GOURAUD, DO_TEXTURED };


struct SOFTWARERASTERIZER_DX10_OBJECTS
{
	ID3D10Device*				pD3D10Device;
	IDXGISwapChain*				pD3D10SwapChain;
	ID3D10RenderTargetView*     pD3D10RenderTargetView;
	ID3D10InputLayout*			pInputLayout;
	ID3D10Effect*				pD3D10Effect;
	ID3D10EffectTechnique*      pD3D10EffectTechnique;
	ID3D10Buffer*				pD3D10VertexBuffer;
	ID3D10Buffer*				pD3D10IndexBuffer;
	ID3D10InfoQueue*			pD3D10InfoQueue;
	ID3D10SamplerState*			pD3D10TextureSampler;
	ID3D10BlendState*			pBlendState;
	DXGI_SWAP_CHAIN_DESC		D3D10SwapChainDesc;
	D3D10_VIEWPORT				D3D10Viewport;

	ID3D10EffectMatrixVariable* pViewMatrixEffectVariable;
	ID3D10EffectMatrixVariable* pWorldMatrixEffectVariable;
	ID3D10EffectMatrixVariable* pProjectionMatrixEffectVariable;

	ID3D10ShaderResourceView*			textureSRV;
	ID3D10EffectShaderResourceVariable* textureSV;
	ID3D10Texture2D*					texture;

	XMFLOAT4X4	matView;
	XMFLOAT4X4	matProjection;
	XMFLOAT4X4  matWorld;

	int			numVertices;
	int			numIndices;

	HWND		mainWindow;
	HINSTANCE	hInstance;
	RECT		clientRect;
	float		drawOffsetX, drawOffsetY;
};
extern SOFTWARERASTERIZER_DX10_OBJECTS SoftwareRasterizer;



struct LinePrimitive
{
	VECTOR2D point0;
	VECTOR2D point1;
	DWORD color0;
	DWORD color1;
};

class SRasterizer
{
public:
	SRasterizer(HWND Window);
	~SRasterizer();


	void DrawLine(DWORD* buffer, int lpitch32, int height, int x0, int y0, int x1, int y1, DWORD color0, DWORD color1, RECT* clipRect = NULL);
	void DrawTriangle(DWORD* video_mem, int lpitch32, VERTEX2D triangle[3], MATRIX3D& transform, int drawOptions, Bitmap* texture = NULL);
	void DrawBitmapWithClipping(Bitmap* bitmap, int destPosX, int destPosY, RECT* sourceRegion = NULL);

private:
	void _DrawFlatTopTriangleTextured(VERTEX2D sortVerts[], MATRIX3D& transform, Bitmap* texture, DWORD* video_mem, int lpitch32);
	void _DrawFlatBottomTriangleTextured(VERTEX2D sortVerts[], MATRIX3D& transform, Bitmap* texture, DWORD* video_mem, int lpitch32);
	void _DrawFlatTopTriangleConstant(VERTEX2D sortVerts[], MATRIX3D& transform, DWORD* video_mem, int lpitch32);
	void _DrawFlatBottomTriangleConstant(VERTEX2D sortVerts[], MATRIX3D& transform, DWORD* video_mem, int lpitch32);
	void _DrawFlatTopTriangleGouraud(VERTEX2D sortVerts[], MATRIX3D& transform, DWORD* video_mem, int lpitch32);
	void _DrawFlatBottomTriangleGouraud(VERTEX2D sortVerts[], MATRIX3D& transform, DWORD* video_mem, int lpitch32);


private:
	ID3D10Device*				pD3D10Device;
	IDXGISwapChain*				pD3D10SwapChain;
	ID3D10RenderTargetView*     pD3D10RenderTargetView;
	ID3D10InputLayout*			pInputLayout;
	ID3D10Effect*				pD3D10Effect;
	ID3D10EffectTechnique*      pD3D10EffectTechnique;
	ID3D10Buffer*				pD3D10VertexBuffer;
	ID3D10Buffer*				pD3D10IndexBuffer;
	ID3D10InfoQueue*			pD3D10InfoQueue;
	ID3D10SamplerState*			pD3D10TextureSampler;
	ID3D10BlendState*			pBlendState;
	DXGI_SWAP_CHAIN_DESC		D3D10SwapChainDesc;
	D3D10_VIEWPORT				D3D10Viewport;

	ID3D10EffectMatrixVariable* pViewMatrixEffectVariable;
	ID3D10EffectMatrixVariable* pWorldMatrixEffectVariable;
	ID3D10EffectMatrixVariable* pProjectionMatrixEffectVariable;

	ID3D10ShaderResourceView*			textureSRV;
	ID3D10EffectShaderResourceVariable* textureSV;
	ID3D10Texture2D*					texture;

	XMFLOAT4X4	matView;
	XMFLOAT4X4	matProjection;
	XMFLOAT4X4  matWorld;

	int			numVertices;
	int			numIndices;

	HWND		mainWindow;
	HINSTANCE	hInstance;
	RECT		clientRect;
	float		drawOffsetX, drawOffsetY;
};

//pipeline manipulation
////////////////////////////////////////////////////////////////////////////////
MATRIX2D SetCameraTransform(MATRIX2D);
MATRIX2D SetProjectionTransform(MATRIX2D);
RECT2D   SetClipRectangle(RECT2D);


//software rast initializing functions
////////////////////////////////////////////////////////////////////////////////
void InitializeSoftwareRasterizer();
void ShutdownSoftwareRasterizer();
void IncrementDrawOffset(float x, float y);

//helpers
//////////////////////////////////////////////////////////////////////////////////
void SortVerticesByY(VERTEX2D verts[], int num);
void SplitTriangle(VERTEX2D sortVerts[], VERTEX2D topTriangle[], VERTEX2D bottomTriangle[]);
void GetRGBA32FromDWORD(int& r, int& g, int& b, int& a, DWORD color);


// queue manipulation
/////////////////////////////////////////////////////////////////////////////////////////////////
void QueueTransformClipLine(float x0, float y0, float x1, float y1, DWORD color0, DWORD color1);
void ClearLineQueue();
void DrawLineQueue(DWORD* buffer, int lpitch);


// vertex rendering
/////////////////////////////////////////////////////////////////////////////////////////////////
void DrawLine(DWORD* buffer, int lpitch32, int height, int x0, int y0, int x1, int y1, DWORD color0, DWORD color1, RECT* clipRect = NULL);

void DrawTriangle(DWORD* video_mem, int lpitch32, VERTEX2D triangle[3], MATRIX3D& transform, int drawOptions, Bitmap* texture = NULL);

	void _DrawFlatTopTriangleTextured(VERTEX2D sortVerts[], MATRIX3D& transform, Bitmap* texture, DWORD* video_mem, int lpitch32);
 
	void _DrawFlatBottomTriangleTextured(VERTEX2D sortVerts[], MATRIX3D& transform, Bitmap* texture, DWORD* video_mem, int lpitch32);
 
	void _DrawFlatTopTriangleConstant(VERTEX2D sortVerts[], MATRIX3D& transform, DWORD* video_mem, int lpitch32);

	void _DrawFlatBottomTriangleConstant(VERTEX2D sortVerts[], MATRIX3D& transform, DWORD* video_mem, int lpitch32);

	void _DrawFlatTopTriangleGouraud(VERTEX2D sortVerts[], MATRIX3D& transform, DWORD* video_mem, int lpitch32);
 
	void _DrawFlatBottomTriangleGouraud(VERTEX2D sortVerts[], MATRIX3D& transform, DWORD* video_mem, int lpitch32);
 

void DrawCircle(DWORD* video_mem, int lpitch32, int xCenter, int yCenter, int radiusPx, DWORD color);

void DrawMeshObject(MESHOBJECT& m, int flags, DWORD* video_mem, int lpitch32);

void DrawBitmapWithClipping(DWORD* dest, int destLPitch32, Bitmap* bitmap, int destPosX, int destPosY, RECT* sourceRegion = NULL);
 

 
 
//---------------------System---------------------------------------
void WindowVisibility(HWND window, bool isVisible);
 
void SystemHotKeys(HWND window);
 

 

//unused

typedef struct SOFTWARERASTERIZER_DX7_OBJECTS_TYP
{
	LPDIRECTDRAW7         lpDDraw7Interface;   // main DirectDraw interface
	LPDIRECTDRAWSURFACE7  lpDDSurfPrimary;     // primary drawing surface
	LPDIRECTDRAWSURFACE7  lpDDSurfBack;		   // secondary surface, acts as a back *buffer
	DDSURFACEDESC2        ddSurfDesc;          // surface description struct, use one for all calls
	DDPIXELFORMAT		  ddPixelFormat;	   // pixel format of the primary surface
	HWND				  mainWindow;
	HINSTANCE			  hInstance;

} SOFTWARERASTERIZER_DX7_OBJECTS;
typedef struct SOFTWARERASTERIZER_DX12_OBJECTS_TYP
{
	/*
	//pipeline
	ComPtr<ID3D12Device>		pD3D12Device;
	ComPtr<IDXGISwapChain3>		pD3D12SwapChain;
	ComPtr<ID3D12Resource>      pD3D12RenderTargets[2];
	ComPtr<ID3D12CommandAllocator> pD3D12CommandAllocator;
	ComPtr<ID3D12CommandQueue> pD3D12CommandQueue;
	ComPtr<ID3D12RootSignature> pD3D12RootSignature;
	ComPtr<ID3D12DescriptorHeap> pD3D12RTVHeap;
	ComPtr<ID3D12PipelineState> pD3D12PipelineState;
	ComPtr<ID3D12GraphicsCommandList> pD3D12CommandList;
	UINT rtvDescriptorSize;
	D3D12_VIEWPORT d3d12Viewport;
	D3D12_RECT	   d3d12ScissorRect;

	//app resources
	ComPtr<ID3D12Resource> pD3D12Vertex*buffer;
	D3D12_VERTEX_*buffer_VIEW d3d12Vertex*bufferView;
	ComPtr<ID3D12Resource> pD3D12Index*buffer;
	D3D12_INDEX_*buffer_VIEW d3d12Index*bufferView;


	//sync
	UINT frameIndex;
	HANDLE hFenceEvent;
	ComPtr<ID3D12Fence> pD3D12Fence;
	UINT64 fenceValue;

	/*
	ID3D10EffectMatrixVariable* pViewMatrixEffectVariable;
	ID3D10EffectMatrixVariable* pWorldMatrixEffectVariable;
	ID3D10EffectMatrixVariable* pProjectionMatrixEffectVariable;

	ID3D10ShaderResourceView*			textureSRV;
	ID3D10EffectShaderResourceVariable* textureSV;
	ID3D10Texture2D*					texture;

	*/

	XMFLOAT4X4	matView;
	XMFLOAT4X4	matProjection;
	XMFLOAT4X4  matWorld;

	int			numVertices;
	int			numIndices;

	HWND		mainWindow;
	HINSTANCE	hInstance;
	RECT		clientRect;

}SOFTWARERASTERIZER_DX12_OBJECTS;
extern SOFTWARERASTERIZER_DX7_OBJECTS softrest7_obj;
extern SOFTWARERASTERIZER_DX12_OBJECTS softrest12_obj;


void InitializeSoftwareRasterizer(SOFTWARERASTERIZER_DX7_OBJECTS* softObjPtr);

void ShutdownSoftwareRasterizer(SOFTWARERASTERIZER_DX7_OBJECTS* softObjPtr);

void InitializeSoftwareRasterizer(SOFTWARERASTERIZER_DX12_OBJECTS* softObjPtr);

void ShutdownSoftwareRasterizer(SOFTWARERASTERIZER_DX12_OBJECTS* softObjPtr);
