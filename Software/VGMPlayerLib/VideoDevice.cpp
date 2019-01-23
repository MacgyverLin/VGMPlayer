#include "VideoDevice.h"
#ifdef STM32
#else
#include <stdio.h>
#include <SDL_main.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

class VideoDeviceImpl
{
public:
#ifdef STM32
#else	
	SDL_Window* window;
	SDL_GLContext glContext;
#endif
};

VideoDevice::VideoDevice()
{
	impl = new VideoDeviceImpl();
}

VideoDevice::~VideoDevice()
{
	if (impl)
	{
		delete impl;
		impl = 0;
	}
}

boolean VideoDevice::open(const string& name_, u32 x_, u32 y_, u32 width_, u32 height_)
{
#ifdef STM32
#else	
	// Create impl->window
	impl->window = SDL_CreateWindow(name_.c_str(), x_, y_, width_, height_, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (impl->window == NULL)
	{
		// Display error message
		//	printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return FALSE;
	}

	// Create OpenGL context
	impl->glContext = SDL_GL_CreateContext(impl->window);
	if (impl->glContext == NULL)
	{
		// Display error message
		//printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return FALSE;
	}

	SDL_GL_SetSwapInterval(1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
#endif

	return TRUE;
}

void VideoDevice::close()
{
#ifdef STM32
#else	
	if (impl->window)
	{
		SDL_GL_DeleteContext(impl->glContext);
		impl->glContext = NULL;

		SDL_DestroyWindow(impl->window);
		impl->window = NULL;
	}
#endif
}

void VideoDevice::makeCurrent()
{
#ifdef STM32
#else	
	SDL_GL_MakeCurrent(impl->window, impl->glContext);
#endif
}

boolean VideoDevice::isCurrent()
{
#ifdef STM32
	return TRUE;
#else	
	SDL_GLContext currentGLContext = SDL_GL_GetCurrentContext();

	return currentGLContext == impl->glContext;
#endif
}

void VideoDevice::flush()
{
#ifdef STM32
#else		
	SDL_GL_SwapWindow(impl->window);
#endif
}

void VideoDevice::clear(const Color& c)
{
#ifdef STM32
#else		
	glClearColor(c.r, c.g, c.b, c.a);
	glClear(GL_COLOR_BUFFER_BIT);
#endif
}

void VideoDevice::drawPoint(const Vertex& v, const Color& c)
{
#ifdef STM32
#else		
	glColor4f(c.r, c.g, c.b, c.a);
	glBegin(GL_POINTS);
	glVertex2fv((f32*)&v);
	glEnd();
#endif
}

void VideoDevice::drawLine(const Vertex& v0, const Vertex& v1, const Color& c)
{
#ifdef STM32
#else		
	glBegin(GL_LINES);

	glColor4fv((f32*)&c);

	glVertex2fv((f32*)&v0);
	glVertex2fv((f32*)&v1);

	glEnd();
#endif
}

void VideoDevice::drawLine(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1)
{
#ifdef STM32
#else		
	glBegin(GL_LINES);

	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glEnd();
#endif
}

void VideoDevice::drawWireTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Color& c)
{
#ifdef STM32
#else		
	glBegin(GL_LINE_LOOP);

	glColor4fv((f32*)&c);

	glVertex2fv((f32*)&v0);
	glVertex2fv((f32*)&v1);
	glVertex2fv((f32*)&v2);

	glEnd();
#endif
}

void VideoDevice::drawWireTriangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2)
{
#ifdef STM32
#else		
	glBegin(GL_LINE_LOOP);

	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glEnd();
#endif
}

void VideoDevice::drawWireRectangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3, const Color& c)
{
#ifdef STM32
#else		
	glBegin(GL_LINE);

	glColor4fv((f32*)&c);

	glVertex2fv((f32*)&v0);
	glVertex2fv((f32*)&v1);
	glVertex2fv((f32*)&v2);
	glVertex2fv((f32*)&v3);

	glEnd();
#endif
}

void VideoDevice::drawWireRectangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2, const Vertex& v3, const Color& c3)
{
#ifdef STM32
#else		
	glBegin(GL_LINES);

	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glColor4fv((f32*)&c3);
	glVertex2fv((f32*)&v3);

	glEnd();
#endif
}

void VideoDevice::drawWireCircle(const Vertex& center, f32 radius, const Color& c)
{
}

void VideoDevice::drawSolidTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Color& c)
{
#ifdef STM32
#else		
	glBegin(GL_TRIANGLES);

	glColor4fv((f32*)&c);
	glVertex2fv((f32*)&v0);

	glColor4fv((f32*)&c);
	glVertex2fv((f32*)&v1);

	glColor4fv((f32*)&c);
	glVertex2fv((f32*)&v2);

	glEnd();
#endif
}

void VideoDevice::drawSolidTriangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2)
{
#ifdef STM32
#else		
	glBegin(GL_TRIANGLES);

	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glEnd();
#endif
}

void VideoDevice::drawSolidRectangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3, const Color& c)
{
#ifdef STM32
#else		
	glBegin(GL_QUADS);

	glColor4fv((f32*)&c);

	glVertex2fv((f32*)&v0);
	glVertex2fv((f32*)&v1);
	glVertex2fv((f32*)&v2);
	glVertex2fv((f32*)&v3);

	glEnd();
#endif
}

void VideoDevice::drawSolidRectangle(const Vertex& v0, const Color& c0, const Vertex& v1, const Color& c1, const Vertex& v2, const Color& c2, const Vertex& v3, const Color& c3)
{
#ifdef STM32
#else		
	glBegin(GL_QUADS);

	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glColor4fv((f32*)&c3);
	glVertex2fv((f32*)&v3);

	glEnd();
#endif
}

void VideoDevice::drawSolidCircle(const Vertex& center, f32 radius, const Color& c)
{
}

void VideoDevice::drawPrimitive(u32 primitive, const Vertex* vertices, const Color& c, u32 count)
{
#ifdef STM32
#else		
	glVertexPointer(2, GL_FLOAT, 0, vertices);

	glDrawArrays(primitive, 0, count);
#endif
}

void VideoDevice::drawPrimitive(u32 primitive, const Vertex* vertices, const Color* colors, u32 count)
{
#ifdef STM32
#else		
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glColorPointer(4, GL_FLOAT, 0, colors);

	glDrawArrays(primitive, 0, count);
#endif
}
