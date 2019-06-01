#pragma once
#include <d3d10.h>
#include "D3DX\D3DX10.h"
#include <DirectXMath.h>
#include <d3dcompiler.h>


#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }
#define HandleError(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
																 \
		}                                                      \
	}

template<typename T>
__forceinline T Min(const T& a, const T& b)
{
	return a < b ? a : b;
}