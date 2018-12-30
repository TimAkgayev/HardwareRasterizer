#pragma once
#include <string>
#include <Windows.h>


class Bitmap
{
public:


	Bitmap(int rows, int pitch, unsigned char* buffer);
	Bitmap(int width, int height);
	Bitmap(std::string fileName);
	~Bitmap();
	Bitmap& operator=(const Bitmap& rhv);

	void FlipBitmap();
	void ResizeBitmap(RECT resizeRectangle);
	void ResizeBitmap(float xscale, float yscale);
	void RotateBitmapLeft();

	void SetName(std::string);

	POINT GetDimensions() const;
	int   GetDataSize() const;
	const BITMAPFILEHEADER& GetFileHeader() const;
	const BITMAPINFOHEADER& GetInfoHeader() const;
	UCHAR* GetData() const;
	const PALETTEENTRY* GetPalette() const;
	std::string GetName() const;


private:
	int LoadBitmapFromDisk(std::string szFileName);
	int Unload_Bitmap_File();

	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	PALETTEENTRY	 palette[256];
	UCHAR*			 data;

	std::string name;


};

