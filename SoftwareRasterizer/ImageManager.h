#pragma once
#include "BitmapImage.h"
#include <vector>
#include <string>
#include <map>



class ImageManager
{
public:

	ImageManager();
	~ImageManager();

	static int AddImage(Bitmap* file);
	static int LoadImageFromFile(std::string path);
	static std::vector<int> LoadImagesFromSprite(std::string path, int StartRow, int StartCol, int EndRow, int EndCol, int cell_width, int cell_height, int border_width = 1);

	static Bitmap* GetImage(int key);
	static Bitmap* GetImage(std::string name);



private:
	static std::map<int, Bitmap*> bmpImageMap;
	static int totalBytesLoaded;

};
