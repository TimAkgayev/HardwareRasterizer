#include "Bitmap.h"

int Bitmap::LoadBitmapFromDisk(std::string szFileName)
{

	// Open the bitmap file
	HANDLE hFile = CreateFileA(szFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return 0;

	// Read the bitmap file header
	DWORD dwBytesRead;
	BOOL bOK = ReadFile(hFile, &fileHeader, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL);

	//see if the correct byte number was read and that it's actually a bitmap (bfType has to match)
	if (!bOK || (dwBytesRead != sizeof(BITMAPFILEHEADER)) || (fileHeader.bfType != 0x4D42))
	{
		CloseHandle(hFile);
		return 0;
	}


	// Read the bitmap info head
	bOK = ReadFile(hFile, &infoHeader, sizeof(BITMAPINFOHEADER), &dwBytesRead, NULL);
	if (!bOK || (dwBytesRead != sizeof(BITMAPINFOHEADER)))
	{
		CloseHandle(hFile);
		return 0;
	}

	// Store the width and height of the bitmap
	int m_iWidth = (int)infoHeader.biWidth;
	int m_iHeight = (int)infoHeader.biHeight;


	SetFilePointer(hFile, fileHeader.bfOffBits, NULL, FILE_BEGIN);
	data = (UCHAR*) new unsigned char[infoHeader.biSizeImage];
	bOK = ReadFile(hFile, data, infoHeader.biSizeImage, &dwBytesRead, NULL);

	//flip the bytes to little endian
	DWORD* temp = (DWORD*)data;
	for (unsigned int i = 0; i < infoHeader.biSizeImage / 4; i++)
	{
		//photoshop saves a,r,g,b into little endian b,g,r,a
		//this engine is expecting r,g,b,a so all that's needed is to flip 1st and 3rd bytes
		//be careful with this code if the image source program is working in r,g,b,a instead of a,r,g,b
		char* byteMem = (char*)&temp[i];
		char btemp = byteMem[0];
		byteMem[0] = byteMem[2];
		byteMem[2] = btemp;

	}

	if (bOK)
		return 1;



	// Something went wrong, so cleanup everything
	return 0;

}

int Bitmap::Unload_Bitmap_File()
{
	// this function releases all memory associated with the bitmap
	if (data)
	{
		// release memory
		delete (data);

		// reset pointer
		data = nullptr;
	}

	return 1;
}

void Bitmap::FlipBitmap()
{
	if (infoHeader.biBitCount == 32)
	{
		DWORD* workingBuffer = (DWORD*) new UCHAR[infoHeader.biSizeImage];
		DWORD* workingBufferStart = workingBuffer;
		DWORD* sourceBuffer = (DWORD*)data;

		//go to the end of the source and go backwards
		sourceBuffer += infoHeader.biWidth * (infoHeader.biHeight - 1);

		for (int row = infoHeader.biHeight - 1; row >= 0; row--)
		{
			memcpy(workingBuffer, sourceBuffer, infoHeader.biWidth * (infoHeader.biBitCount / 8));

			workingBuffer += infoHeader.biWidth;
			sourceBuffer -= infoHeader.biWidth;
		}

		memcpy(data, workingBufferStart, infoHeader.biSizeImage);
		delete workingBufferStart;

	}



}

void Bitmap::ResizeBitmap(RECT resizeRectangle)
{


	int newXDimension = resizeRectangle.right - resizeRectangle.left;
	int newYDimension = resizeRectangle.bottom - resizeRectangle.top;



	int newDataSize = (infoHeader.biBitCount / 8) * newXDimension * newYDimension;




	//layout of bmp
	//file header
	//dib header (info header)
	//data

	//first create a new file header
	//everything is the same as the old bitmap except the file size
	BITMAPFILEHEADER newFH = fileHeader;
	newFH.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + newFH.bfOffBits + newDataSize;


	//create a new info header
	BITMAPINFOHEADER newIH = infoHeader;
	newIH.biSize = sizeof(BITMAPINFOHEADER);
	newIH.biHeight = newYDimension;
	newIH.biWidth = newXDimension;
	newIH.biSizeImage = newDataSize;


	//calculate the resize ratio in x dimension
	float ratioX = float(newXDimension) / infoHeader.biWidth;
	float ratioY = float(newYDimension) / infoHeader.biHeight;

	UCHAR* newData = 0;
	if (infoHeader.biBitCount == 32)
	{
		//allocate the data (this is only for 32 bit bitmap)
		DWORD* tempData;
		tempData = (DWORD*) new DWORD[newXDimension * newYDimension];


		DWORD* newDataOffset = tempData;
		DWORD* origDataOffset = (DWORD*)data;

		//iterate throught the original bitmap and write to the new one
		for (int row = 0; row < infoHeader.biHeight; row++)
		{
			for (int column = 0; column < infoHeader.biWidth; column++)
			{
				int destinationRow = int(ratioY * row);
				int destinationPixel = int(ratioX * column);
				newDataOffset[destinationRow* newXDimension + destinationPixel] = origDataOffset[column];
			}

			//newDataOffset += newXDimension;
			origDataOffset += infoHeader.biWidth;
		}

		newData = (UCHAR*)tempData;
	}
	else if (infoHeader.biBitCount == 8)
	{
		//allocate the data 
		UCHAR* tempData;
		tempData = new UCHAR[newXDimension * newYDimension];

		UCHAR* newDataOffset = tempData;
		UCHAR* origDataOffset = (UCHAR*)data;

		//iterate throught the original bitmap and write to the new one
		for (int y = 0; y < infoHeader.biHeight; y++)
		{
			for (int x = 0; x < infoHeader.biWidth; x++)
			{
				int destinationRow = int(ratioY * y);
				int destinationPixel = int(ratioX * x);
				newDataOffset[destinationRow* newXDimension + destinationPixel] = origDataOffset[x];
			}

			//newDataOffset += newXDimension;
			origDataOffset += infoHeader.biWidth;
		}

		newData = (UCHAR*)tempData;
	}

	//delete the old data
	delete data;


	//copy everything back to the output bitmap
	fileHeader = newFH;
	infoHeader = newIH;
	data = (unsigned char*)newData;





}
void Bitmap::ResizeBitmap(float xscale, float yscale)
{
	//calculate the size based on percentage
	int newXDim = int(infoHeader.biWidth * xscale + 0.5f);
	int newYDim = int(infoHeader.biHeight * yscale + 0.5f);
	RECT rc;
	rc.left = 0;
	rc.right = newXDim;
	rc.top = 0;
	rc.bottom = newYDim;

	ResizeBitmap(rc);
}

void Bitmap::RotateBitmapLeft()
{
	//allocate temporary data
	DWORD* tempData = (DWORD*) new char[infoHeader.biSizeImage];

	//create new info header
	int newXDim = infoHeader.biHeight;
	int newYDim = infoHeader.biWidth;

	DWORD* origData = (DWORD*)data;

	//iterate throught the original bitmap and write to the new one
	for (int row = 0; row < infoHeader.biHeight; row++)
	{
		for (int columnOld = 0, columnNew = infoHeader.biWidth - 1; columnOld < infoHeader.biWidth, columnNew >= 0; columnOld++, columnNew--)
		{

			tempData[columnNew*newXDim + row] = origData[columnOld];
		}

		//newDataOffset += newXDimension;
		origData += infoHeader.biWidth;
	}

	//copy over the temp data
	memcpy(data, tempData, infoHeader.biSizeImage);
	infoHeader.biWidth = newXDim;
	infoHeader.biHeight = newYDim;

	delete tempData;


}

Bitmap::Bitmap(std::string filename)
{
	LoadBitmapFromDisk(filename);
	size_t pos = filename.find_last_of('\\');
	name = filename.substr(pos + 1);
	int x = 0;

}

Bitmap::Bitmap(int width, int height)
{
	memset(&fileHeader, 0, sizeof(BITMAPFILEHEADER));
	memset(&infoHeader, 0, sizeof(BITMAPINFOHEADER));

	//set up info
	infoHeader.biBitCount = 32;
	infoHeader.biClrImportant = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biCompression = BI_RGB;
	infoHeader.biHeight = height;
	infoHeader.biWidth = width;
	infoHeader.biPlanes = 1;
	infoHeader.biSize = sizeof(BITMAPINFOHEADER);
	infoHeader.biSizeImage = width*height * sizeof(DWORD);
	infoHeader.biXPelsPerMeter = 2835;
	infoHeader.biYPelsPerMeter = 2835;

	//allocate space
	data = new UCHAR[infoHeader.biSizeImage];

	//now set up file header last
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 1024);
	fileHeader.bfSize = infoHeader.biSize + sizeof(BITMAPFILEHEADER) + infoHeader.biSizeImage;
	fileHeader.bfType = 'BM';

}

Bitmap::Bitmap(int width, int height, unsigned char* buffer)
{
	memset(&fileHeader, 0, sizeof(BITMAPFILEHEADER));
	memset(&infoHeader, 0, sizeof(BITMAPINFOHEADER));

	//set up info
	infoHeader.biBitCount = 8;
	infoHeader.biClrImportant = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biCompression = BI_RLE8;
	infoHeader.biHeight = height;
	infoHeader.biWidth = width;
	infoHeader.biPlanes = 1;
	infoHeader.biSize = sizeof(BITMAPINFOHEADER);
	infoHeader.biSizeImage = width*height;
	infoHeader.biXPelsPerMeter = 2835;
	infoHeader.biYPelsPerMeter = 2835;

	//allocate space
	data = new UCHAR[infoHeader.biSizeImage];

	//now set up file header last
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 1024);
	fileHeader.bfSize = infoHeader.biSize + sizeof(BITMAPFILEHEADER) + infoHeader.biSizeImage;
	fileHeader.bfType = 'BM';

	memcpy(data, buffer, infoHeader.biSizeImage);
}


Bitmap::~Bitmap()
{
	if (data)
		delete data;

	data = nullptr;

}

Bitmap& Bitmap::operator=(const Bitmap& rhv)
{
	if (data)
		delete data;

	data = nullptr;
	memset(&fileHeader, 0, sizeof(BITMAPFILEHEADER));
	memset(&infoHeader, 0, sizeof(BITMAPINFOHEADER));

	fileHeader = rhv.fileHeader;
	infoHeader = rhv.infoHeader;

	data = new UCHAR[rhv.GetDataSize()];
	memcpy(data, rhv.GetData(), rhv.GetDataSize());

	return (*this);
}



void Bitmap::SetName(std::string n)
{
	name = n;
}

const PALETTEENTRY* Bitmap::GetPalette() const
{
	return palette;
}

POINT Bitmap::GetDimensions() const
{
	POINT p = { infoHeader.biWidth, infoHeader.biHeight };
	return p;
}
int   Bitmap::GetDataSize() const
{
	return infoHeader.biSizeImage;
}
const BITMAPFILEHEADER& Bitmap::GetFileHeader() const
{
	return fileHeader;
}
const BITMAPINFOHEADER& Bitmap::GetInfoHeader() const
{
	return infoHeader;
}
UCHAR* Bitmap::GetData() const
{
	return data;
}




std::string Bitmap::GetName() const
{
	return name;
}




