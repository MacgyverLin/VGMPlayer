#ifndef _VideoDevice_h_
#define _VideoDevice_h_

#include "vgmdef.h"
#include <SDL_main.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <GL/gl.h>
using namespace std;

class Vertex
{
public:
	Vertex(FLOAT32 x_, FLOAT32 y_)
	{
		x = x_;
		y = y_;
	}

	FLOAT32 x;
	FLOAT32 y;
};

class Color
{
public:
	Color(FLOAT32 r_, FLOAT32 g_, FLOAT32 b_, FLOAT32 a_)
	{
		r = r_;
		g = g_;
		b = b_;
		a = a_;
	}

	FLOAT32 r;
	FLOAT32 g;
	FLOAT32 b;
	FLOAT32 a;
};

class VideoDevice
{
public:
	VideoDevice();
	~VideoDevice();

	BOOL open(UINT32 x_, UINT32 y_, UINT32 width_, UINT32 height_);
	VOID close();

	void makeCurrent();
	BOOL isCurrent();
	void flush();

	VOID clear(const Color& c);

	VOID drawPoint(const Vertex& v, const Color& c);
	VOID drawLine(const Vertex& v0, const Vertex& v1, const Color& c);
	VOID drawLine(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1);
	VOID drawWireTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Color& c);
	VOID drawWireTriangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2);
	VOID drawWireRectangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3, const Color& c);
	VOID drawWireRectangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2, const Vertex& v3, const Color& c3);
	VOID drawWireCircle(const Vertex& center, FLOAT32 radius, const Color& c);

	VOID drawSolidTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Color& c);
	VOID drawSolidTriangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2);
	VOID drawSolidRectangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3, const Color& c);
	VOID drawSolidRectangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2, const Vertex& v3, const Color& c3);
	VOID drawSolidCircle(const Vertex& center, FLOAT32 radius, const Color& c);

	VOID drawPrimitive(const Vertex* vertices, const Color& c, UINT32 count);
	VOID drawPrimitive(const Vertex* vertices, const Color* colors, UINT32 count);
protected:
private:
////////////////////////////////////////////////////////////////////////////
public:
protected:
private:
	SDL_Window* window;
	SDL_GLContext glContext;
};

#endif