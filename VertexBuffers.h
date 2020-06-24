#pragma once
#include "common_includes.h"
#include "Vertex.h"
using namespace DirectX;

namespace VertexBuffers
{

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