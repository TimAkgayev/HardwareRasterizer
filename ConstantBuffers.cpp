#include "ConstantBuffers.h"

namespace ConstantBuffers
{
	ID3D11Buffer*  ViewProjBuffer = nullptr;
	ID3D11Buffer*  WorldMatrixBuffer = nullptr;
	ID3D11Buffer*  DirectionalLightBuffer = nullptr;
	

	void InitAll(ID3D11Device* device)
	{
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(ProjectionVariables);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		device->CreateBuffer(&bufferDesc, NULL, &ViewProjBuffer);


		bufferDesc.ByteWidth = sizeof(WorldMatrices);
		device->CreateBuffer(&bufferDesc, NULL, &WorldMatrixBuffer);


		bufferDesc.ByteWidth = sizeof(DirectionalLight);
		device->CreateBuffer(&bufferDesc, NULL, &DirectionalLightBuffer);
		
	}
}


