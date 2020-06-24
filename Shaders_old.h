#pragma once
#include "common_includes.h"
using namespace DirectX;




namespace DX11Pipeline
{
	typedef unsigned int UINT;
	namespace Vertex
	{


		struct PosColor
		{

			XMFLOAT3 pos;
			XMFLOAT4 color;
		};

		struct Pos
		{
			XMFLOAT3 pos;
		};

		struct PosNormTexcoord
		{
			XMFLOAT3 pos;
			XMFLOAT3 norm;
			XMFLOAT2 uv;
		};
	};


	namespace VertexBuffers
	{
		typedef unsigned int UINT;

		class InputAssemblerObject
		{
		public:
			virtual ~InputAssemblerObject();

			virtual UINT GetNumberOfIndices() const;
			virtual ID3D11Buffer* GetVertexBuffer() const;
			virtual ID3D11Buffer* GetIndexBuffer() const;
			virtual ID3D11InputLayout* GetInputLayout() const;

			virtual D3D11_PRIMITIVE_TOPOLOGY GetTopology() const;
			virtual UINT GetStride() const = 0;
			virtual DXGI_FORMAT GetIndexBufferFormat() const;

			virtual void SetInputAssemblerObject(ID3D11DeviceContext* context);
		protected:

			UINT mNumVertices;
			UINT mNumIndices;

			ID3D11Buffer* mVB;
			ID3D11Buffer* mIB;

			D3D11_PRIMITIVE_TOPOLOGY mTopology;
			DXGI_FORMAT mIndexBufferFormat;

			ID3D11InputLayout* mInputLayout;
		};

		class VertexBuffer_P : public InputAssemblerObject
		{
		public:
			VertexBuffer_P();
			~VertexBuffer_P();

			virtual void Initialize(const Vertex::Pos* vertexList, UINT num_vert, UINT* indexList, UINT num_ind);
			virtual UINT GetStride() const override;

		};

		class VertexBuffer_PC : public InputAssemblerObject
		{
		public:
			VertexBuffer_PC();
			~VertexBuffer_PC();

			virtual void Initialize(const Vertex::PosColor* vertexList, UINT num_vert, UINT* indexList, UINT num_ind);
			virtual UINT GetStride() const override;

		};

		class VertexBuffer_PNT : public InputAssemblerObject
		{
		public:
			VertexBuffer_PNT();
			~VertexBuffer_PNT();

			virtual void Initialize(const Vertex::PosNormTexcoord* vertexList, UINT num_vert, UINT* indexList, UINT num_ind);
			virtual UINT GetStride() const override;

		};
	}

	

	class Sky
	{
	public:
		Sky(ID3D11Device* device, ID3D11DeviceContext* context,  UINT screenWidth, UINT screenHeight);

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

	

	


	class SimpleColorPipeline
	{
	public:
		SimpleColorPipeline(ID3D11Device* device, UINT screenWidth, UINT screenHeight);

		virtual void Render(ID3D11DeviceContext* context, VertexBuffers::VertexBuffer_PC* vertex_buffer) override;

		void SetConstantBuffer_ViewProjection(ID3D11DeviceContext* context, XMMATRIX& view, XMMATRIX& proj);
		void SetConstantBuffer_World(ID3D11DeviceContext* context, XMMATRIX& world);

	private:
		//constant buffers
		ID3D11Buffer* mConstantBuffer_ViewProj;
		ID3D11Buffer* mConstantBuffer_World;

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

	

	
		

	class DirectionalLightAndShadow
	{
	public:
		DirectionalLightAndShadow(ID3D11Device* device, UINT screenWidth, UINT screenHeight);

		void Render(ID3D11DeviceContext * context, DirectXPipeline::VertexBuffers::VertexBuffer_PNT * vertex_buffer_surface, DirectXPipeline::VertexBuffers::VertexBuffer_PNT * vertex_buffer_objects);
		//void SetShaderResources()

	private:

		_vs_shadowMap mVS_ShadowMap;
		_vs_directionalLight mVS_DirectionalLight;
		_ps_shadowMap mPS_ShadowMap;
		_ps_shadowSurfaceDirectionalLight mPS_ShadowSurfaceDirectionalLight;
		_ps_directionalLight mPS_DirectionalLight;



		//Hull Stage->Tessalator Shader
		//Domain Stage
		//Geometry Stage->Stream Output Stage

		//Rasterizer Stage
		D3D11_VIEWPORT Viewport;
		ID3D11RasterizerState* RasterizerState;
		D3D11_RECT ScissorRect;



		//Output Merger Stage
		ID3D11RenderTargetView* RenderTargetView;
		ID3D11DepthStencilState* DepthStencilState;
		ID3D11Texture2D* DepthStencilTexture;
		ID3D11DepthStencilView* DepthStencilView;
		ID3D11BlendState* BlendState;

		//shadow variables
		ID3D11ShaderResourceView* mDepthStenceilSRV;
		ID3D11SamplerState* mShadowSamplerState;


	};



	namespace ConstantBuffers
	{
		
		__declspec(align(16)) struct DirectionalLightVariables
		{
			XMFLOAT3   LightDirection;
			float    _padding1;
			XMFLOAT4   LightColor;
			XMMATRIX LightWorldMatrix;
			XMMATRIX LightViewMatrix;
			XMMATRIX LightProjectionMatrix;
			XMFLOAT3   LightPosition;
			float    _padding2;

		};

	}

	namespace Shaders
	{


		class BaseShader
		{
		public:
			BaseShader(ID3D11Device* device, std::wstring filename, std::string entryPoint);
			virtual void SetShader(ID3D11DeviceContext* context) = 0;

		protected:
			ID3DBlob*	mByteCode;
			ID3D11VertexShader*  mShader;

		protected:
			HRESULT mCompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
			HRESULT mCreateShaderFromBlob(ID3D11Device* device, ID3DBlob* blob, ID3D11VertexShader** vs);
			HRESULT mCreateShaderFromBlob(ID3D11Device* device, ID3DBlob* blob, ID3D11PixelShader** vs);


		};

		class _vs_shadowMap : public BaseShader
		{
		public:
			_vs_shadowMap(ID3D11Device* device, std::wstring filename, std::string entryPoint);

			void SetConstantBuffer_World(ID3D11DeviceContext* context, XMMATRIX& world);
			void SetConstantBuffer_LightVariables(ID3D11DeviceContext* context, XMFLOAT4 Position, XMFLOAT3 Direction, XMFLOAT4 Color, XMMATRIX LightWorldMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix);

			virtual void SetShader(ID3D11DeviceContext* context) override;
		private:
			ID3D11Buffer* mConstantBuffer_World;
			ID3D11Buffer* mConstantBuffer_LightVariables;


		};

		class _ps_shadowMap : public BaseShader
		{
		public:
			_ps_shadowMap(ID3D11Device* device, std::wstring filename, std::string entryPoint);
			virtual void SetShader(ID3D11DeviceContext* context) override;

		};

		class _vs_directionalLight : public BaseShader
		{
		public:
			_vs_directionalLight(ID3D11Device* device, std::wstring filename, std::string entryPoint);
			virtual void SetShader(ID3D11DeviceContext* context) override;

			void SetConstantBuffer_World(ID3D11DeviceContext* context, XMMATRIX& world);
			void SetConstantBuffer_LightVariables(ID3D11DeviceContext* context, XMFLOAT4 Position, XMFLOAT3 Direction, XMFLOAT4 Color, XMMATRIX LightWorldMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix);
			void SetConstantBuffer_ViewProjection(ID3D11DeviceContext* context, XMMATRIX & view, XMMATRIX& proj);
			void SetConstantBuffer_CameraPosition(ID3D11DeviceContext* context, XMFLOAT4& CamPosition);


		private:
			ID3D11Buffer* mConstantBuffer_World;
			ID3D11Buffer* mConstantBuffer_ViewProjection;
			ID3D11Buffer* mConstantBuffer_LightVariables;
			ID3D11Buffer* mConstantBuffer_CameraPosition;

		};

		class _ps_shadowSurfaceDirectionalLight : public BaseShader
		{
		public:
			_ps_shadowSurfaceDirectionalLight(ID3D11Device* device, std::wstring filename, std::string entryPoint);
			virtual void SetShader(ID3D11DeviceContext* context) override;

		};
		class _ps_directionalLight : public BaseShader
		{
		public:
			_ps_directionalLight(ID3D11Device* device, std::wstring filename, std::string entryPoint);
			virtual void SetShader(ID3D11DeviceContext* context) override;

		};

	}



};
