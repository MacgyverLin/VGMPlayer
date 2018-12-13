#include <stdio.h>
#include "VideoDevice.h"

VideoDevice::VideoDevice()
{
}

VideoDevice::~VideoDevice()
{
}

BOOL VideoDevice::open(UINT32 x_, UINT32 y_, UINT32 width_, UINT32 height_)
{
	// Create window
	window = SDL_CreateWindow("Hello World!", x_, y_, width_, height_, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
	if (window == NULL)
	{
		// Display error message
		//	printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// Create OpenGL context
	glContext = SDL_GL_CreateContext(window);
	if (glContext == NULL)
	{
		// Display error message
		//printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	SDL_GL_SetSwapInterval(1);

	//glViewport(x_, y_, width_, height_);

	return true;
}

VOID VideoDevice::close()
{
	if(window)
	{
		// SDL_GL_DestroyContext(glContext);
		glContext = NULL;

		SDL_DestroyWindow(window);
		window = NULL;
	}
}

void VideoDevice::makeCurrent()
{
	SDL_GL_MakeCurrent(window, glContext);
}

BOOL VideoDevice::isCurrent()
{
	SDL_GLContext currentGLContext = SDL_GL_GetCurrentContext();

	return currentGLContext== glContext;
}

void VideoDevice::flush()
{
	SDL_GL_SwapWindow(window);
}

VOID VideoDevice::clear(const Color& c)
{
	glClearColor(c.r, c.g, c.b, c.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

VOID VideoDevice::drawPoint(const Vertex& v, const Color& c)
{
	glColor4f(c.r, c.g, c.b, c.a);
	glBegin(GL_POINTS);
		glVertex2f(v.x, v.y);
	glEnd();
}

VOID VideoDevice::drawLine(const Vertex& v0, const Vertex& v1, const Color& c)
{
	glBegin(GL_LINES);

	glColor4fv((FLOAT32*)&c);
	glVertex2f(v0.x, v0.y);
	glVertex2f(v1.x, v1.y);
	
	glEnd();
}

VOID VideoDevice::drawLine(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1)
{
	glBegin(GL_LINES);
	
	glColor4fv((FLOAT32*)&c0);
	glVertex2f(v0.x, v0.y);
	glColor4fv((FLOAT32*)&c1);
	glVertex2f(v1.x, v1.y);
	
	glEnd();
}

VOID VideoDevice::drawWireTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Color& c)
{
	glBegin(GL_LINE_LOOP);

	glColor4fv((FLOAT32*)&c);

	glVertex2f(v0.x, v0.y);
	glVertex2f(v1.x, v1.y);
	glVertex2f(v2.x, v2.y);

	glEnd();
}

VOID VideoDevice::drawWireTriangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2)
{
	glBegin(GL_LINE_LOOP);

	glColor4fv((FLOAT32*)&c0);
	glVertex2f(v0.x, v0.y);
	glColor4fv((FLOAT32*)&c1);
	glVertex2f(v1.x, v1.y);
	glColor4fv((FLOAT32*)&c2);
	glVertex2f(v2.x, v2.y);

	glEnd();
}

VOID VideoDevice::drawWireRectangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3, const Color& c)
{
	glBegin(GL_LINE);

	glColor4fv((FLOAT32*)&c);

	glVertex2f(v0.x, v0.y);
	glVertex2f(v1.x, v1.y);
	glVertex2f(v2.x, v2.y);
	glVertex2f(v3.x, v3.y);

	glEnd();
}

VOID VideoDevice::drawWireRectangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2, const Vertex& v3, const Color& c3)
{
	glBegin(GL_LINES);

	glColor4fv((FLOAT32*)&c0);
	glVertex2f(v0.x, v0.y);
	glColor4fv((FLOAT32*)&c1);
	glVertex2f(v1.x, v1.y);
	glColor4fv((FLOAT32*)&c2);
	glVertex2f(v2.x, v2.y);
	glColor4fv((FLOAT32*)&c3);
	glVertex2f(v3.x, v3.y);

	glEnd();
}

VOID VideoDevice::drawWireCircle(const Vertex& center, FLOAT32 radius, const Color& c)
{
}

VOID VideoDevice::drawSolidTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Color& c)
{
}

VOID VideoDevice::drawSolidTriangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2)
{
}

VOID VideoDevice::drawSolidRectangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3, const Color& c)
{
}

VOID VideoDevice::drawSolidRectangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2, const Vertex& v3, const Color& c3)
{
}

VOID VideoDevice::drawSolidCircle(const Vertex& center, FLOAT32 radius, const Color& c)
{
}

VOID VideoDevice::drawPrimitive(const Vertex* vertices, const Color& c, UINT32 count)
{
}

VOID VideoDevice::drawPrimitive(const Vertex* vertices, const Color* colors, UINT32 count)
{
}