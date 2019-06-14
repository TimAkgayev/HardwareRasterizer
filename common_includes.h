#pragma once
#include <Windows.h>
#include <Windows.Foundation.h>
#include <wrl\wrappers\corewrappers.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <string>
#include <vector>

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