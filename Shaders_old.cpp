#include "Shaders.h"
#include "Utility.h"
#include <iostream>
using namespace DirectX;

namespace DX11Pipeline
{
	namespace VertexBuffers
	{
		

		InputAssemblerObject::~InputAssemblerObject()
		{
			ReleaseCOM(mVB);
			ReleaseCOM(mIB);
		}

		UINT InputAssemblerObject::GetNumberOfIndices() const
		{
			return mNumIndices;
		}

		ID3D11Buffer * InputAssemblerObject::GetVertexBuffer() const
		{
			return mVB;
		}

		ID3D11Buffer * InputAssemblerObject::GetIndexBuffer() const
		{
			return mIB;
		}

		ID3D11InputLayout * InputAssemblerObject::GetInputLayout() const
		{
			return mInputLayout;
		}

		D3D11_PRIMITIVE_TOPOLOGY InputAssemblerObject::GetTopology() const
		{
			return mTopology;
		}

		DXGI_FORMAT InputAssemblerObject::GetIndexBufferFormat() const
		{
			return mIndexBufferFormat;
		}

		void InputAssemblerObject::SetInputAssemblerObject(ID3D11DeviceContext * context)
		{
			ID3D11Buffer* vb = mVB;
			UINT stride = GetStride();
			UINT offset = 0;
			context->IASetInputLayout(mInputLayout);
			context->IASetPrimitiveTopology(mTopology);
			context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
			context->IASetIndexBuffer(mIB, mIndexBufferFormat, 0);
		}


		VertexBuffer_P::VertexBuffer_P()
		{
		}
		VertexBuffer_P::~VertexBuffer_P()
		{
		}
		void VertexBuffer_P::Initialize(const Vertex::Pos * vertexList, UINT num_vert, UINT * indexList, UINT num_ind)
		{
		}
		UINT VertexBuffer_P::GetStride() const
		{
			return sizeof(Vertex::Pos);
		}
		VertexBuffer_PNT::VertexBuffer_PNT()
		{
		}
		VertexBuffer_PNT::~VertexBuffer_PNT()
		{
		}
		void VertexBuffer_PNT::Initialize(const Vertex::PosNormTexcoord * vertexList, UINT num_vert, UINT * indexList, UINT num_ind)
		{
		}
		UINT VertexBuffer_PNT::GetStride() const
		{
			return sizeof(Vertex::PosNormTexcoord);
		}
		VertexBuffer_PC::VertexBuffer_PC()
		{
		}
		VertexBuffer_PC::~VertexBuffer_PC()
		{
		}
		void VertexBuffer_PC::Initialize(const Vertex::PosColor * vertexList, UINT num_vert, UINT * indexList, UINT num_ind)
		{
		}
		UINT VertexBuffer_PC::GetStride() const
		{
			return sizeof(Vertex::PosColor);
		}

	}

	Sky(ID3D11Device * device, UINT screenWidth, UINT screenHeight)
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

	void Pipeline::SkyboxPipeline::Render(ID3D11DeviceContext * context, VertexBuffers::VertexBuffer_P * vertex_buffer)
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


	void Pipeline::SkyboxPipeline::SetConstantBuffer_ViewProjection(ID3D11DeviceContext* context, XMMATRIX & view, XMMATRIX & proj)
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

	void Pipeline::SkyboxPipeline::SetConstantBuffer_CameraPosition(ID3D11DeviceContext* context, XMFLOAT3& eyePos)
	{
		__declspec(align(16)) struct
		{
			DirectX::XMFLOAT3 EyePosition;
			float _padding;
		}var;

		var.EyePosition = eyePos;

		context->UpdateSubresource(mConstantBuffer_CameraPosition, 0, NULL, &var, 0, 0);
	}


}



D3D11_VIEWPORT DirectXPipeline::BasePipeline::mCreateViewport(UINT width, UINT height)
{
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	return viewport;
}

HRESULT DirectXPipeline::Shaders::BaseShader::mCompileShaderFromFile(WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob ** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags | = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_PREFER_FLOW_CONTROL;
#endif


	ID3DBlob* errors;

	hr = D3DCompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &errors);

	if (errors)
	{
		OutputDebugStringA(reinterpret_cast<const char*>(errors->GetBufferPointer()));
		errors->Release();
	}


	return hr;
}

void DirectXPipeline::PNTPipelines::DirectionalLightAndShadow::Render(ID3D11DeviceContext * context, DirectXPipeline::VertexBuffers::VertexBuffer_PNT * vertex_buffer_surface, DirectXPipeline::VertexBuffers::VertexBuffer_PNT * vertex_buffer_objects)
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
	vertex_buffer_surface->SetInputAssemblerObject(context);

	//vertex shader stage
	mVS_ShadowMap.SetShader(context);

	//hull stage
	//domain stage
	//geometry stage

	//rasterizer stage
	context->RSSetViewports(1, &Viewport);
	context->RSSetScissorRects(1, &ScissorRect);
	context->RSSetState(RasterizerState);

	//pixel shader stage
	mPS_ShadowMap.SetShader(context);



	//output merger stage
	ID3D11RenderTargetView* oldRenderTarget;
	ID3D11DepthStencilView* oldDepthStencil;
	context->OMGetRenderTargets(1, &oldRenderTarget, &oldDepthStencil);
	context->OMSetBlendState(BlendState, 0, 0xffffffff);
	context->OMSetDepthStencilState(DepthStencilState, 0);
	context->OMSetRenderTargets(0, NULL, DepthStencilView); // Only bind the ID3D11DepthStencilView for output

	context->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	context->DrawIndexed(vertex_buffer_surface->GetNumberOfIndices(), 0, 0);


	//now render the shadows of the objects
	vertex_buffer_objects->SetInputAssemblerObject(context);

	context->DrawIndexed(vertex_buffer_objects->GetNumberOfIndices(), 0, 0);

	//now render the actual geometry
	vertex_buffer_surface->SetInputAssemblerObject(context);
	mVS_DirectionalLight.SetShader(context);
	mPS_ShadowSurfaceDirectionalLight.SetShader(context);
	context->OMSetRenderTargets(1, &oldRenderTarget, oldDepthStencil);
	context->DrawIndexed(vertex_buffer_surface->GetNumberOfIndices(), 0, 0);

	vertex_buffer_objects->SetInputAssemblerObject(context);
	mPS_DirectionalLight.SetShader(context);
	context->DrawIndexed(vertex_buffer_objects->GetNumberOfIndices(), 0, 0);
}







ID3D11VertexShader * Pipeline::BasePipeline::GetVertexShader() const
{
	return VertexShader;
}

ID3D11PixelShader * Pipeline::BasePipeline::GetPixelShader() const
{
	return PixelShader;
}

ID3DBlob * Pipeline::BasePipeline::GetVSBlob() const
{
	return Blob_VertexShader;
}

ID3DBlob * Pipeline::BasePipeline::GetPSBlob() const
{
	return Blob_PixelShader;
}


Pipeline::PCPipelines::SimpleColor::SimpleColor(ID3D11Device* device, UINT screenWidth, UINT screenHeight)
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


	mCompileFilesToBlobs(TEXT("DX10HardwareRendererEffect.fx"), "VS_ColorVertex", "PS_ColorVertex");
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
	struct view_proj_buffer
	{
		XMMATRIX view;
		XMMATRIX proj;
	};

	struct world_buffer
	{
		XMMATRIX world;
	};

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(view_proj_buffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&bufferDesc, NULL, &mConstantBuffer_ViewProj);
	bufferDesc.ByteWidth = sizeof(world_buffer);
	device->CreateBuffer(&bufferDesc, NULL, &mConstantBuffer_World);
}

void Pipeline::SimpleColorShader::Render(ID3D11DeviceContext * context, VertexBuffers::VertexBuffer_PC * vertex_buffer)
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
	context->VSSetConstantBuffers(1, 1, &mConstantBuffer_World);




	//hull stage
	//domain stage
	//geometry stage

	//rasterizer stage
	context->RSSetViewports(1, &Viewport);
	context->RSSetScissorRects(1, &ScissorRect);
	context->RSSetState(RasterizerState);

	//pixel shader stage
	context->PSSetShader(PixelShader, NULL, 0);
	context->PSSetConstantBuffers(0, 1, &mConstantBuffer_ViewProj);
	context->PSSetConstantBuffers(1, 1, &mConstantBuffer_World);


	//output merger stage
	context->OMSetBlendState(BlendState, 0, 0xffffffff);
	context->OMSetDepthStencilState(DepthStencilState, 0);
	context->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

	context->DrawIndexed(vertex_buffer->GetNumberOfIndices(), 0, 0);
}

void Pipeline::SimpleColorShader::SetConstantBuffer_ViewProjection(ID3D11DeviceContext* context, XMMATRIX & view, XMMATRIX & proj)
{
	struct
	{
		XMMATRIX view;
		XMMATRIX proj;
	}vars;

	vars.view = view;
	vars.proj = proj;

	context->UpdateSubresource(mConstantBuffer_ViewProj, 0, NULL, &vars, 0, 0);

}

void Pipeline::SimpleColorShader::SetConstantBuffer_World(ID3D11DeviceContext* context, XMMATRIX & world)
{
	__declspec(align(16)) struct
	{
		XMMATRIX world;
	}vars;

	vars.world = world;

	context->UpdateSubresource(mConstantBuffer_World, 0, NULL, &vars, 0, 0);
}


Pipeline::ShadowMapShader::ShadowMapShader(ID3D11Device* device, UINT screenWidth, UINT screenHeight)
{
	//init variables
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


	//setup the viewport
	Viewport = mCreateViewport(screenWidth, screenHeight);

	ID3D11DeviceContext* context;
	device->GetImmediateContext(&context);

	//create a stencil-depth texture 
	D3D11_TEXTURE2D_DESC shadowMapDesc;
	ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
	shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.ArraySize = 1;
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	shadowMapDesc.Height = screenHeight;
	shadowMapDesc.Width = screenWidth;
	HandleError(device->CreateTexture2D(&shadowMapDesc, nullptr, &DepthStencilTexture));

	//create a view for the stencil-depth buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	HandleError(device->CreateDepthStencilView(DepthStencilTexture, &depthStencilViewDesc, &DepthStencilView));

	//create a shader view for the stencil-depth bufffer
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	HandleError(device->CreateShaderResourceView(DepthStencilTexture, &shaderResourceViewDesc, &mDepthStenceilSRV));

	//create a sampler state
	D3D11_SAMPLER_DESC comparisonSamplerDesc;
	ZeroMemory(&comparisonSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.BorderColor[0] = 1.0f;
	comparisonSamplerDesc.BorderColor[1] = 1.0f;
	comparisonSamplerDesc.BorderColor[2] = 1.0f;
	comparisonSamplerDesc.BorderColor[3] = 1.0f;
	comparisonSamplerDesc.MinLOD = 0.f;
	comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	comparisonSamplerDesc.MipLODBias = 0.f;
	comparisonSamplerDesc.MaxAnisotropy = 0;
	comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;

	// Point filtered shadows can be faster, and may be a good choice when
	// rendering on hardware with lower feature levels. This sample has a
	// UI option to enable/disable filtering so you can see the difference
	// in quality and speed.
	device->CreateSamplerState(&comparisonSamplerDesc, &mShadowSamplerState);


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



	//set up scissor rect
	ScissorRect.left = 0;
	ScissorRect.right = screenWidth;
	ScissorRect.top = 0;
	ScissorRect.bottom = screenHeight;
}

Pipeline::BaseShader::BaseShader(ID3D11Device* device, std::wstring filename, std::string entryPoint)
{
	mCompileShaderFromFile((WCHAR*)filename.c_str(), "VS_ShadowMap", "vs_5_0", &mByteCode);
	mCreateShaderFromBlob(device, mByteCode, &mShader);
	
}

Pipeline::_vs_shadowMap::_vs_shadowMap(ID3D11Device * device, std::wstring filename, std::string entryPoint)
	:BaseShader(device, filename, entryPoint)
{
	
	__declspec(align(16)) struct
	{
		XMFLOAT3   LightDirection;
		float    _directional_light_padding;
		XMFLOAT4   LightColor;
		XMMATRIX LightWorldMatrix;
		XMMATRIX LightViewMatrix;
		XMMATRIX LightProjectionMatrix;
		XMFLOAT4   LightPosition;
	}light_cbuffer;

	__declspec(align(16)) struct
	{
		XMMATRIX world;
	}world_cbuffer;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(light_cbuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&bufferDesc, NULL, &mConstantBuffer_LightVariables);
	bufferDesc.ByteWidth = sizeof(world_cbuffer);
	device->CreateBuffer(&bufferDesc, NULL, &mConstantBuffer_World);
}

void Pipeline::_vs_shadowMap::SetConstantBuffer_World(ID3D11DeviceContext* context, XMMATRIX & world)
{
	__declspec(align(16)) struct
	{
		XMMATRIX world;
	}vars;

	vars.world = world;

	context->UpdateSubresource(mConstantBuffer_World, 0, NULL, &vars, 0, 0);
}

void Pipeline::_vs_shadowMap::SetConstantBuffer_LightVariables(ID3D11DeviceContext* context, XMFLOAT4 Position, XMFLOAT3 Direction, XMFLOAT4 Color, XMMATRIX LightWorldMatrix, XMMATRIX LightViewMatrix, XMMATRIX LightProjectionMatrix)
{
	__declspec(align(16)) struct
	{
		XMFLOAT3   LightDirection;
		float    _directional_light_padding;
		XMFLOAT4   LightColor;
		XMMATRIX LightWorldMatrix;
		XMMATRIX LightViewMatrix;
		XMMATRIX LightProjectionMatrix;
		XMFLOAT4   LightPosition;
	}vars;

	vars.LightDirection = Direction;
	vars.LightColor = Color;
	vars.LightPosition = Position;
	vars.LightProjectionMatrix = LightProjectionMatrix;
	vars.LightViewMatrix = LightViewMatrix;
	vars.LightWorldMatrix = LightWorldMatrix;

	context->UpdateSubresource(mConstantBuffer_LightVariables, 0, NULL, &vars, 0, 0);

}


void Pipeline::_vs_directionalLight::SetConstantBuffer_World(ID3D11DeviceContext* context, XMMATRIX & world)
{
	__declspec(align(16)) struct
	{
		XMMATRIX world;
	}vars;

	vars.world = world;

	context->UpdateSubresource(mConstantBuffer_World, 0, NULL, &vars, 0, 0);
}

void Pipeline::_vs_directionalLight::SetConstantBuffer_LightVariables(ID3D11DeviceContext* context, XMFLOAT4 Position, XMFLOAT3 Direction, XMFLOAT4 Color, XMMATRIX LightWorldMatrix, XMMATRIX LightViewMatrix, XMMATRIX LightProjectionMatrix)
{
	ConstantBuffers::DirectinalLightVariables vars;

	vars.LightDirection = Direction;
	vars.LightColor = Color;
	vars.LightPosition = Position;
	vars.LightProjectionMatrix = LightProjectionMatrix;
	vars.LightViewMatrix = LightViewMatrix;
	vars.LightWorldMatrix = LightWorldMatrix;

	context->UpdateSubresource(mConstantBuffer_LightVariables, 0, NULL, &vars, 0, 0);

}

void Pipeline::_vs_directionalLight::SetConstantBuffer_ViewProjection(ID3D11DeviceContext* context, XMMATRIX & view, XMMATRIX& proj)
{
	__declspec(align(16)) struct
	{
		XMMATRIX world;
		XMMATRIX proj;
	}vars;

	vars.world = view;
	vars.proj = proj;

	context->UpdateSubresource(SetConstantBuffer_ViewProjection, 0, NULL, &vars, 0, 0);
}


void Pipeline::_vs_directionalLight::SetConstantBuffer_ViewProjection(ID3D11DeviceContext* context, XMFLOAT3& CameraPosition)
{
	__declspec(align(16)) struct
	{
		XMMATRIX world;
		XMMATRIX proj;
	}vars;

	vars.world = view;
	vars.proj = proj;

	context->UpdateSubresource(SetConstantBuffer_ViewProjection, 0, NULL, &vars, 0, 0);
}

void Pipeline::_vs_shadowMap::SetShader(ID3D11DeviceContext * context)
{
	context->VSSetShader(mShader, NULL, 0);
	context->VSSetConstantBuffers(1, 1, &mConstantBuffer_World);
	context->VSSetConstantBuffers(2, 1, &mConstantBuffer_LightVariables);
	
}

Pipeline::_ps_shadowMap::_ps_shadowMap(ID3D11Device * device, std::wstring filename, std::string entryPoint)
	:BaseShader(device, filename, entryPoint)
{

}

void Pipeline::_ps_shadowMap::SetShader(ID3D11DeviceContext * context)
{
}

Pipeline::_vs_directionalLight::_vs_directionalLight(ID3D11Device * device, std::wstring filename, std::string entryPoint)
	:BaseShader(device, filename, entryPoint)
{
	__declspec(align(16)) struct
	{
		XMFLOAT3   LightDirection;
		float     _directional_light_padding;
		XMFLOAT4   LightColor;
		XMMATRIX   LightWorldMatrix;
		XMMATRIX   LightViewMatrix;
		XMMATRIX   LightProjectionMatrix;
		XMFLOAT4   LightPosition;
	}light_cbuffer;

	__declspec(align(16)) struct
	{
		XMMATRIX world;
	}world_cbuffer;

	__declspec(align(16)) struct
	{
		XMMATRIX view;
		XMMATRIX proj;

	}viewProj_cbuffer;

	__declspec(align(16)) struct
	{
		XMFLOAT3 EyePosition;
		float padding;
	}camPos_cbuffer;


	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	bufferDesc.ByteWidth = sizeof(light_cbuffer);
	device->CreateBuffer(&bufferDesc, NULL, &mConstantBuffer_LightVariables);

	bufferDesc.ByteWidth = sizeof(world_cbuffer);
	device->CreateBuffer(&bufferDesc, NULL, &mConstantBuffer_World);

	bufferDesc.ByteWidth = sizeof(viewProj_cbuffer);
	device->CreateBuffer(&bufferDesc, NULL, &mConstantBuffer_ViewProjection);

	bufferDesc.ByteWidth = sizeof(camPos_cbuffer);
	device->CreateBuffer(&bufferDesc, NULL, &mConstantBuffer_CameraPosition);
}

void Pipeline::_vs_directionalLight::SetShader(ID3D11DeviceContext * context)
{
}

Pipeline::_ps_shadowSurfaceDirectionalLight::_ps_shadowSurfaceDirectionalLight(ID3D11Device * device, std::wstring filename, std::string entryPoint)
	:BaseShader(device, filename, entryPoint)
{
}

void Pipeline::_ps_shadowSurfaceDirectionalLight::SetShader(ID3D11DeviceContext * context)
{
}

Pipeline::_ps_directionalLight::_ps_directionalLight(ID3D11Device * device, std::wstring filename, std::string entryPoint)
	:BaseShader(device, filename, entryPoint)
{
}

void Pipeline::_ps_directionalLight::SetShader(ID3D11DeviceContext * context)
{
}
