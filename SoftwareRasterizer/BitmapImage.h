#pragma once
#include "Bitmap.h"
#include "Math.h"
#include <vector>;

class BitmapImage
{
public:

	BitmapImage();
	BitmapImage(int RMKey);
	BitmapImage(Bitmap*);
	~BitmapImage();

	void AddFrame(Bitmap* bmpPtr);
	void AddFrame(int RMKey);
	void Draw(DWORD* vmem, int lpitch32);

	void SetFramerate(int framerate);
	void SetPosition(VECTOR2D& pos);
	void SetPosition(float x, float y);
	void SetName(std::string name);

	void GetFrameDimensions(POINT&) const;
	void GetBoundingRectangle(RECT2D&);
	int GetFrame(int frame);
	std::string GetName();
	int GetNumFrames();
	VECTOR2D GetPosition() const;

	void RemoveFrame(int);

	void RotateLeft();

	bool CheckCollision(RECT&);
private:
	std::vector<Bitmap*> frames;
	VECTOR2D position;
	float timeOverflow;
	double currentTime;
	int currentFrame;
	bool isClockStarted;
	LARGE_INTEGER endTime, startTime, perfFreq;
	int mID;
	std::string mName;

	int GetID();
};


