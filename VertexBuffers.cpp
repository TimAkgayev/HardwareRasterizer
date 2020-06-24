#include "VertexBuffers.h"
#include "Utility.h"

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
