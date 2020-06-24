#include "SkyboxEffect.h"
#include <d3d11.h>


SkyboxEffect(D3D11Device* device, UINT screenWidth, UINT screenHeight)
{
	ID3D11VertexShader*  VertexShader = nullptr;
	ID3D11PixelShader*   PixelShader = nullptr;

	ID3DBlob*	Blob_VertexShader = nullptr;
	ID3DBlob*   Blob_PixelShader = nullptr;

	memset(&Viewport, 0, sizeof(D3D11_VIEWPORT));
	memset(&ScissorRect, 0, sizeof(D3D11_RECT));
	ID3D11RasterizerState* RasterizerState = nullptr;
	ID3D11RenderTargetView* RenderTargetView = nullptr;
	ID3D11DepthStencilState* DepthStencilState = nullptr;
	ID3D11Texture2D* DepthStencilTexture = nullptr;
	ID3D11DepthStencilView* DepthStencilView = nullptr;
	ID3D11BlendState* BlendState = nullptr;


	mCompileFilesToBlobs(TEXT("DX10HardwareRendererEffect.fx"), "VS_Skybox", "PS_Skybox");
	mCreateShadersFromBlobs(device);

	Viewport = mCreateViewport(screenWidth, screenHeight);

	D3D11_RASTERIZER_DESC RasterizerStateDesc;
	RasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerStateDesc.CullMode = D3D11_CULL_FRONT;
	RasterizerStateDesc.FrontCounterClockwise = true;
	RasterizerStateDesc.DepthBias = false;
	RasterizerStateDesc.DepthBiasClamp = 0;
	RasterizerStateDesc.SlopeScaledDepthBias = 0;
	RasterizerStateDesc.DepthClipEnable = true;
	RasterizerStateDesc.ScissorEnable = true;
	RasterizerStateDesc.MultisampleEnable = false;
	RasterizerStateDesc.AntialiasedLineEnable = false;

	device->CreateRasterizerState(&RasterizerStateDesc, &RasterizerState);

	ScissorRect.left = 0;
	ScissorRect.right = screenWidth;
	ScissorRect.top = 0;
	ScissorRect.bottom = screenHeight;

	//set up depth-stencil buffer
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	device->CreateDepthStencilState(&dsDesc, &DepthStencilState);


	//create the actual depth-stencil texture
	DepthStencilTexture = nullptr;
	D3D11_TEXTURE2D_DESC depthDesc;
	depthDesc.Width = screenWidth;
	depthDesc.Height = screenHeight;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	device->CreateTexture2D(&depthDesc, nullptr, &DepthStencilTexture);


	//set up depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	// Create the depth stencil view
	device->CreateDepthStencilView(DepthStencilTexture, &descDSV, &DepthStencilView);

	// Setup blending for transparent textures
	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&blendStateDesc, &BlendState);

	//setup constant buffers
	__declspec(align(16)) struct
	{
		XMMATRIX view;
		XMMATRIX proj;
	}view_proj_buffer;

	__declspec(align(16)) struct
	{
		XMMATRIX world;
	}cameraposition_buffer;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(view_proj_buffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&bufferDesc, NULL, &mConstantBuffer_ViewProj);
	bufferDesc.ByteWidth = sizeof(cameraposition_buffer);
	device->CreateBuffer(&bufferDesc, NULL, &mConstantBuffer_CameraPosition);


}

void SkyboxEffect::Render(ID3D11DeviceContext * context, VertexBuffers::VertexBuffer_P * vertex_buffer)
{
	/*
	Input Assembler
	Vertex Stage
	Hull Stage -> Tessalator Shader
	Domain Stage
	Geometry Stage -> Stream Output Stage
	Rasterizer Stage
	Pixel Stage
	Output Merger Stage
	*/


	//input assembler stage
	ID3D11Buffer* vb = vertex_buffer->GetVertexBuffer();
	UINT stride = vertex_buffer->GetStride();
	UINT offset = 0;
	context->IASetInputLayout(vertex_buffer->GetInputLayout());
	context->IASetPrimitiveTopology(vertex_buffer->GetTopology());
	context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	context->IASetIndexBuffer(vertex_buffer->GetIndexBuffer(), vertex_buffer->GetIndexBufferFormat(), 0);

	//vertex shader stage
	context->VSSetShader(VertexShader, NULL, 0);
	context->VSSetConstantBuffers(0, 1, &mConstantBuffer_ViewProj);
	context->VSSetConstantBuffers(5, 1, &mConstantBuffer_CameraPosition);


	//hull stage
	//domain stage
	//geometry stage

	//rasterizer stage
	context->RSSetViewports(1, &Viewport);
	context->RSSetScissorRects(1, &ScissorRect);
	context->RSSetState(RasterizerState);

	//pixel shader stage
	context->PSSetShader(PixelShader, NULL, 0);



	//output merger stage
	context->OMSetBlendState(BlendState, 0, 0xffffffff);
	context->OMSetDepthStencilState(DepthStencilState, 0);
	context->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

	context->DrawIndexed(vertex_buffer->GetNumberOfIndices(), 0, 0);
}


void SkyboxEffect::SetConstantBuffer_ViewProjection(ID3D11DeviceContext* context, XMMATRIX & view, XMMATRIX & proj)
{
	__declspec(align(16)) struct
	{
		XMMATRIX view;
		XMMATRIX proj;
	}vars;

	vars.view = view;
	vars.proj = proj;

	context->UpdateSubresource(mConstantBuffer_ViewProj, 0, NULL, &vars, 0, 0);

}

void SkyboxEffect::SetConstantBuffer_CameraPosition(ID3D11DeviceContext* context, XMFLOAT3& eyePos)
{
	__declspec(align(16)) struct
	{
		DirectX::XMFLOAT3 EyePosition;
		float _padding;
	}var;

	var.EyePosition = eyePos;

	context->UpdateSubresource(mConstantBuffer_CameraPosition, 0, NULL, &var, 0, 0);
}


