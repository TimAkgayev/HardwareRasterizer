#include "Interface_Application.h"

wchar_t* Application::GetWindowTitle()
{
	return mWindowTitle;
}

int Application::GetClientHeight()
{
	return mClientHeight;
}

int Application::GetClientWidth()
{
	return mClientWidth;
}

void* Application::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void Application::operator delete(void* p)
{
	_mm_free(p);
}

void Application::SetRasterizer(RasterizerInterface * rasterizer)
{
	mRasterizer = rasterizer;
}
