#ifndef _VideoDevice_h_
#define _VideoDevice_h_

#include "vgmdef.h"
#include <string>
#include <algorithm>
using namespace std;

class Vector2
{
public:
	Vector2(f32 x_=0, f32 y_ = 0)
	{
		x = x_;
		y = y_;
	}

	f32 x;
	f32 y;
};

class Color
{
public:
	Color(f32 r_=0.0f, f32 g_ = 0.0f, f32 b_ = 0.0f, f32 a_ = 0.0f)
	{
		r = r_;
		g = g_;
		b = b_;
		a = a_;
	}

	friend Color operator * (const Color& c, f32 scale)
	{
		Color result;
		
		result.r = c.r * scale;
		result.g = c.g * scale;
		result.b = c.b * scale;
		result.a = c.a * scale;

		return result;
	}

	Color& operator *= (const f32& scale)
	{
		this->r *= scale;
		this->g *= scale;
		this->b *= scale;
		this->a *= scale;
		return *this;
	}

	f32 r;
	f32 g;
	f32 b;
	f32 a;
};

class VideoDeviceImpl;

class Texture2D
{
public:
	Texture2D();
	~Texture2D();

	void Load(const char* path_);
	void Bind() const;
private:
	unsigned int handle;
};

class VideoDevice
{
public:
	VideoDevice();
	~VideoDevice();

	boolean open(const string& name_, u32 x_, u32 y_, u32 width_, u32 height_);
	void close();

	void makeCurrent();
	boolean isCurrent();
	void flush();

	void clear(const Color& c);

	void drawPoint(const Vector2& v, const Color& c);
	void drawLine(const Vector2& v0, const Color& c0, const Vector2& v1, const Color& c1);
	void drawWireTriangle(const Vector2& v0, const Color& c0, const Vector2& v1, const Color& c1, const Vector2& v2, const Color& c2);
	void drawWireRectangle(const Vector2& v0, const Color& c0, const Vector2& v1, const Color& c1, const Vector2& v2, const Color& c2, const Vector2& v3, const Color& c3);
	void drawWireCircle(const Vector2& center, f32 radius, const Color& c);

	void drawSolidTriangle(const Vector2& v0, const Color& c0, const Vector2& v1, const Color& c1, const Vector2& v2, const Color& c2);
	void drawSolidRectangle(const Vector2& v0, const Color& c0, const Vector2& v1, const Color& c1, const Vector2& v2, const Color& c2, const Vector2& v3, const Color& c3);
	void drawSolidCircle(const Vector2& center, f32 radius, const Color& c);

	void drawPrimitive(u32 primitive, const Vector2* vertices, const Color* colors, u32 count);

	//////////////////////////////////////////////////////////////
	void drawTexPoint(const Texture2D& texture, const Vector2& v, const Color& c, const Vector2& t);
	void drawTexLine(const Texture2D& texture, const Vector2& v0, const Color& c0, const Vector2& t0, const Vector2& v1, const Color& c1, const Vector2& t1);
	void drawTexWireTriangle(const Texture2D& texture, 
								const Vector2& v0, const Color& c0, const Vector2& t0, 
							    const Vector2& v1, const Color& c1, const Vector2& t1, 
								const Vector2& v2, const Color& c2, const Vector2& t2);
	void drawTexWireRectangle(const Texture2D& texture, 
								const Vector2& v0, const Color& c0, const Vector2& t0, 
								const Vector2& v1, const Color& c1, const Vector2& t1, 
								const Vector2& v2, const Color& c2, const Vector2& t2,
								const Vector2& v3, const Color& c3, const Vector2& t3);

	void drawTexSolidTriangle(const Texture2D& texture, 
								const Vector2& v0, const Color& c0, const Vector2& t0, 
								const Vector2& v1, const Color& c1, const Vector2& t1, 
								const Vector2& v2, const Color& c2, const Vector2& t2);
	void drawTexSolidRectangle(const Texture2D& texture, 
								const Vector2& v0, const Color& c0, const Vector2& t0, 
								const Vector2& v1, const Color& c1, const Vector2& t1, 
								const Vector2& v2, const Color& c2, const Vector2& t2, 
								const Vector2& v3, const Color& c3, const Vector2& t4);

	void drawTexPrimitive(const Texture2D& texture, u32 primitive, const Vector2* v, const Color* c, const Vector2* t, u32 count);
protected:
private:
////////////////////////////////////////////////////////////////////////////
public:
protected:
private:
	VideoDeviceImpl* impl;
};

#endif
