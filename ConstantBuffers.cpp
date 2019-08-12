#include "ConstantBuffers.h"

namespace ConstantBuffers
{
	ID3D11Buffer*  ViewProjBuffer = nullptr;
	ID3D11Buffer*  WorldMatrixBuffer = nullptr;
	ID3D11Buffer*  DirectionalLightBuffer = nullptr;
	ID3D11Buffer*  CameraPositionBuffer = nullptr;
	ID3D11Buffer*  LightVariablesBuffer = nullptr;
	ID3D11Buffer*  MaterialBuffer = nullptr;

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

		bufferDesc.ByteWidth = sizeof(LightVariables);
		device->CreateBuffer(&bufferDesc, NULL, &LightVariablesBuffer);

		bufferDesc.ByteWidth = sizeof(CameraPosition);
		device->CreateBuffer(&bufferDesc, NULL, &CameraPositionBuffer);

		bufferDesc.ByteWidth = sizeof(Material);
		device->CreateBuffer(&bufferDesc, NULL, &MaterialBuffer);

		//bind all the buffers to a device
		ID3D11DeviceContext* deviceContext;
		device->GetImmediateContext(&deviceContext);

		deviceContext->VSSetConstantBuffers(0, 1, &ViewProjBuffer);
		deviceContext->PSSetConstantBuffers(0, 1, &ViewProjBuffer);

		deviceContext->VSSetConstantBuffers(1, 1, &WorldMatrixBuffer);
		deviceContext->PSSetConstantBuffers(1, 1, &WorldMatrixBuffer);


		deviceContext->VSSetConstantBuffers(2, 1, &DirectionalLightBuffer);
		deviceContext->PSSetConstantBuffers(2, 1, &DirectionalLightBuffer);


		deviceContext->VSSetConstantBuffers(3, 1, &MaterialBuffer);
		deviceContext->PSSetConstantBuffers(3, 1, &MaterialBuffer);


		deviceContext->VSSetConstantBuffers(4, 1, &LightVariablesBuffer);
		deviceContext->PSSetConstantBuffers(4, 1, &LightVariablesBuffer);


		deviceContext->VSSetConstantBuffers(5, 1, &CameraPositionBuffer);
		deviceContext->PSSetConstantBuffers(5, 1, &CameraPositionBuffer);

		
	}
}


