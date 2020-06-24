#include "Utility.h"

std::string GetBaseDir(const std::string& filepath) {
	if (filepath.find_last_of("/\\") != std::string::npos)
		return filepath.substr(0, filepath.find_last_of("/\\"));
	return "";
}

std::wstring StringToWString(const std::string & filepath)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(filepath);
}

template<typename T> __forceinline T Min(const T& a, const T& b)
{
	return a < b ? a : b;
}
