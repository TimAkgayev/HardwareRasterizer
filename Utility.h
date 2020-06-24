#pragma once
#include <string>

//for converting std::string to std::wstring
#include <locale>
#include <codecvt>

#define ReleaseCOM(x) { if(x){ x->Release(); x = nullptr; } }
#define HandleError(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
																 \
		}                                                      \
	}

template<typename T>
__forceinline T Min(const T& a, const T& b);

std::string GetBaseDir(const std::string& filepath);
std::wstring StringToWString(const std::string& filepath);



