#pragma once
#include "common_includes.h"

using namespace DirectX;


class SBP
{
public:
	SBP(ID3D11Device* device, ID3D11DeviceContext* context, UINT screenWidth, UINT screenHeight);

	virtual void Render(ID3D11DeviceContext* context, VertexBuffers::VertexBuffer_P* vertex_buffer) override;

	void SetConstantBuffer_ViewProjection(ID3D11DeviceContext* context, XMMATRIX& view, XMMATRIX& proj);
	void SetConstantBuffer_CameraPosition(ID3D11DeviceContext* context, XMFLOAT3& eyePos);

private:
	//constant buffers
	ID3D11Buffer* mConstantBuffer_ViewProj;
	ID3D11Buffer* mConstantBuffer_CameraPosition;

	//Vertex Stage
	ID3D11VertexShader*  VertexShader;

	//Hull Stage->Tessalator Shader
	//Domain Stage
	//Geometry Stage->Stream Output Stage

	//Rasterizer Stage
	D3D11_VIEWPORT Viewport;
	ID3D11RasterizerState* RasterizerState;
	D3D11_RECT ScissorRect;

	//Pixel Stage
	ID3D11PixelShader*   PixelShader;

	//Output Merger Stage
	ID3D11RenderTargetView* RenderTargetView;
	ID3D11DepthStencilState* DepthStencilState;
	ID3D11Texture2D* DepthStencilTexture;
	ID3D11DepthStencilView* DepthStencilView;
	ID3D11BlendState* BlendState;

	//misc
	ID3DBlob*	Blob_VertexShader;
	ID3DBlob*   Blob_PixelShader;

};
