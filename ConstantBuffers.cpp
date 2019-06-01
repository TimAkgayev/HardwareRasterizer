#include "ConstantBuffers.h"

namespace ConstantBuffers
{
	ID3D10Buffer*  ViewWorldProjBuffer = nullptr;

	void InitAll(ID3D10Device* device)
	{
		//create a constant buffer for shader constant manipluation
		D3D10_BUFFER_DESC bufferDesc;
		bufferDesc.MiscFlags = 0;
		bufferDesc.Usage = D3D10_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(ProjectionVariables);
		bufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;

		device->CreateBuffer(&bufferDesc, NULL, &ViewWorldProjBuffer);
	}
}


