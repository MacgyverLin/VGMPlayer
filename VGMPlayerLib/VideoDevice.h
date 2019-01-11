#ifndef _VideoDevice_h_
#define _VideoDevice_h_

#include "vgmdef.h"
#include <string>
#include <algorithm>
using namespace std;

class Platform
{
public:
	static bool initialize();
	static bool update();
	static void terminate();
};

class Vertex
{
public:
	Vertex(f32 x_=0, f32 y_ = 0)
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

	void drawPoint(const Vertex& v, const Color& c);
	void drawLine(const Vertex& v0, const Vertex& v1, const Color& c);
	void drawLine(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1);
	void drawWireTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Color& c);
	void drawWireTriangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2);
	void drawWireRectangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3, const Color& c);
	void drawWireRectangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2, const Vertex& v3, const Color& c3);
	void drawWireCircle(const Vertex& center, f32 radius, const Color& c);

	void drawSolidTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Color& c);
	void drawSolidTriangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2);
	void drawSolidRectangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3, const Color& c);
	void drawSolidRectangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2, const Vertex& v3, const Color& c3);
	void drawSolidCircle(const Vertex& center, f32 radius, const Color& c);

	void drawPrimitive(u32 primitive, const Vertex* vertices, const Color& c, u32 count);
	void drawPrimitive(u32 primitive, const Vertex* vertices, const Color* colors, u32 count);
protected:
private:
////////////////////////////////////////////////////////////////////////////
public:
protected:
private:
	VideoDeviceImpl* impl;
};

#endif
