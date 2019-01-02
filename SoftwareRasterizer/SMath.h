#pragma once

#include <windows.h>
#include <math.h>
#include "MacrosAndDefines.h"
//#include "D3DX10.h"

const float MATH_PI = 3.1415926f;

struct VECTOR2D
{
public:

	VECTOR2D(float inX, float inY);
	VECTOR2D();

	//convinent operators overloaded
	VECTOR2D operator+ (const VECTOR2D& rhs);

	VECTOR2D operator- (const VECTOR2D& rhs);

	VECTOR2D operator* (float scalar);


	bool operator == (const VECTOR2D& rhs);

	void Normalize();
	void MakeNegative();
	float Length();
	float DistanceTo(VECTOR2D v);
	float AngleToVector(VECTOR2D& v);
	float AngleToWorld();

	float x;
	float y;

	static VECTOR2D DOWN_VECTOR();
	static VECTOR2D UP_VECTOR();
	static VECTOR2D LEFT_VECTOR();
	static VECTOR2D RIGHT_VECTOR();


};



class VECTOR3D
{
public:
	VECTOR3D(double inX, double inY, double inZ);
	VECTOR3D();

	//convinent operators overloaded
	VECTOR3D operator+ (const VECTOR3D& rhs);
	VECTOR3D operator- (const VECTOR3D& rhs);
	VECTOR3D operator* (float scalar);

	bool operator == (const VECTOR3D& rhs);
	void Normalize();
	float Length();
	float DistanceTo(VECTOR3D v);

	double x;
	double y;
	double z;
};

class VECTOR4D
{
public:
	VECTOR4D(float inX, float inY, float inZ, float inW);
	VECTOR4D();

	//convinent operators overloaded
	VECTOR4D operator+ (const VECTOR4D& rhs);
	VECTOR4D operator- (const VECTOR4D& rhs);
	VECTOR4D operator* (float scalar);
	bool operator == (const VECTOR4D& rhs);

	void Normalize();
	float Length();

	float x, y, z, w;

};
typedef struct MATRIX1x3_TYP
{

	union
	{
		double M[3]; //array index

		struct
		{
			double _00, _01, _02;
		};
	};

}MATRIX1x3, *MATRIX1x3_PTR;


typedef struct MATRIX2x2_TYP
{
	union
	{
		float M[2][2]; //array index

		struct
		{
			float _00, _01;
			float _10, _11;
		};
	};

}MATRIX2x2, MATRIX2D, *MATRIX2x2_PTR, *MATRIX2D_PTR;

typedef struct MATRIX3x3_TYP
{
	union
	{
		float M[3][3]; //array index

		struct
		{
			float _00, _01, _02;
			float _10, _11, _12;
			float _20, _21, _22;
		};
	};

}MATRIX3x3, MATRIX3D, *MATRIX3x3_PTR, *MATRIX3D_PTR;


typedef struct MATRIX4x4_TYP
{
	/*
	MATRIX4x4_TYP(D3DXMATRIX& m)
	{
	_00 = m._11;
	_01 = m._12;
	_02 = m._13;
	_03 = m._14;

	_10 = m._21;
	_11 = m._22;
	_12 = m._23;
	_13 = m._24;

	_20 = m._31;
	_21 = m._32;
	_22 = m._33;
	_23 = m._34;

	_30 = m._41;
	_31 = m._42;
	_32 = m._43;
	_33 = m._44;

	}

	MATRIX4x4_TYP()
	{

	}

	D3DXMATRIX toDirecXMatrix()
	{
	D3DXMATRIX m;
	m._11 = _00;
	m._12 = _01;
	m._13 = _02;
	m._14 = _03;

	m._21 = _10;
	m._22 = _11;
	m._23 = _12;
	m._24 = _13;

	m._31 = _20;
	m._32 = _21;
	m._33 = _22;
	m._34 = _23;

	m._41 = _30;
	m._42 = _31;
	m._43 = _32;
	m._44 = _33;

	return m;
	}
	*/
	union
	{
		double M[4][4]; //array index

		struct
		{
			double _00, _01, _02, _03;
			double _10, _11, _12, _13;
			double _20, _21, _22, _23;
			double _30, _31, _32, _33;
		};
	};

}MATRIX4x4, MATRIX4D, *MATRIX4x4_PTR, *MATRIX4_PTR;

class RECT2D
{
public:

	RECT2D() :left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {}
	RECT2D(float x, float y, float x1, float y1) : left(x), top(y), right(x1), bottom(y1) {}
	RECT2D(RECT& r) {
		left = r.left;
		right = r.right;
		top = r.top;
		bottom = r.bottom;
	}
	void setWidth(float w)
	{
		right = left + w;

	}

	void setHeight(float h)
	{
		bottom = top + h;
	}

	void setPos(float x, float y)
	{
		right = (right - left) + x;
		bottom = (bottom - top) + y;
		left = x;
		top = y;

	}

	void SetRECT(RECT r)
	{
		left = r.left;
		top = r.top;
		right = r.right;
		bottom = r.bottom;
	}

	void setPos(VECTOR2D& v)
	{
		right = (right - left) + v.x;
		bottom = (bottom - top) + v.y;
		left = v.x;
		top = v.y;
	}

	float getWidth() const
	{
		return right - left;
	}

	float getHeight() const
	{
		return bottom - top;
	}

	void move(VECTOR2D dir, int pxPerSec, float deltaTime)
	{
		left = left + pxPerSec*dir.x * deltaTime;
		right = right + pxPerSec*dir.x * deltaTime;
		top = top + pxPerSec*dir.y * deltaTime;
		bottom = bottom + pxPerSec*dir.y * deltaTime;
	}

	VECTOR2D GetPosition() const
	{
		VECTOR2D v;
		v.x = left;
		v.y = top;

		return v;
	}

	inline bool isPointInRect(float x, float y)
	{
		//reverse signs if the region is negative (made from right to left)
		if (left > right)
		{
			if (x <= left && x >= right)
			if(top > bottom)
			{
				if (y <= top && y >= bottom)
					return true;
			}
			else
			{
				if (y >= top && y <= bottom)
					return true;
			}
		}

		if (x >= left && x <= right)
		{
			if (top > bottom)
			{
				if (y <= top && y >= bottom)
					return true;
			}
			else
			{

				if (y >= top && y <= bottom)
					return true;
			}
		}

		return false;
	}

	bool isRectInRect(RECT& region)
	{
		//test x dimension
		if (region.left <= left)
		{

			if (region.right > left)
			{
				//now check y dimension
				if (region.top <= top)
				{
					if (region.bottom > top)
						return true;
					else return false;
				}
				else if (region.top > top)
				{
					if (region.top < bottom)
						return true;
					else
						return false;
				}
			}
			else
				return false;

		}
		else if (region.left > left)
		{
			if (region.left < right)
			{
				//now check y dimension
				if (region.top <= top)
				{
					if (region.bottom > top)
						return true;
					else return false;
				}
				else if (region.top > top)
				{
					if (region.top < bottom)
						return true;
					else
						return false;
				}
			}
			else
				return false;
		}

		return false;
	}

	RECT GetWINRECT()
	{
		RECT r;
		r.left = int(left + 0.5f);
		r.right = int(right + 0.5f);
		r.top = int(top + 0.5f);
		r.bottom = int(bottom + 0.5f);

		return r;
	}

public:
	float left;
	float top;
	float right;
	float bottom;

};









void MatrixIdentity(MATRIX2D* mat);
void MatrixIdentity(MATRIX3D* mat);
void MatrixIdentity(MATRIX4D* mat);
void MatrixScale(MATRIX4D* mat, float x, float y, float z);
void MatrixScale(MATRIX3D* mat, float x, float y);
void MatrixTranslation(MATRIX4D* mat, float x, float y, float z);
void MatrixTranslation(MATRIX3D* mat, float x, float y);

void Vector2Rotation(VECTOR2D* v, float rad);
void MatrixRotation(MATRIX4D* mat, VECTOR3D axis, float rad);
void MatrixScalarMultiply(MATRIX2D* out, MATRIX2D& in, float scalar);
void MatrixMatrixMultiply(MATRIX4D* out, MATRIX4D* op1, MATRIX4D* op2);
void MatrixMatrixMultiply(MATRIX3D* out, MATRIX3D* op1, MATRIX3D* op2);
int MatrixDeterminant(MATRIX2D& in);
void MatrixInverse(MATRIX2D* out, MATRIX2D& in);

void MatrixVectorMultiply(VECTOR2D* operand, MATRIX2x2_PTR transform, VECTOR2D* out = NULL);
void MatrixVectorMultiply(VECTOR2D* operand, MATRIX3x3_PTR transform, VECTOR2D* out = NULL);
void MatrixVectorMultiply(VECTOR3D* operand, MATRIX4x4_PTR transform, VECTOR3D* out = NULL);
void Vector3CrossProduct(VECTOR3D* out, VECTOR3D in, VECTOR3D in2);


int CohenSutherlandLineClip(int& x1, int& y1, int& x2, int& y2, RECT clipRect);

float GetDistanceToLine(VECTOR2D Lstart, VECTOR2D Lend, VECTOR2D point);

MATRIX1x3 VectorToMatrix(VECTOR2D* v);