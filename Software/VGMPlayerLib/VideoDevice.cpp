#include "VideoDevice.h"
#ifdef STM32
#else
#include <stdio.h>
#include <SDL_main.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <GL/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#endif


Texture2D::Texture2D()
: handle(0)
{
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &handle);
}

void Texture2D::Load(const char* path_)
{
	int width;
	int height;
	int nrComponents;
	void *data = stbi_load(path_, &width, &height, &nrComponents, 0);
	stbi__vertical_flip(data, width, height, nrComponents * 1);

	if (data)
	{
		glGenTextures(1, &handle);

		glBindTexture(GL_TEXTURE_2D, handle);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);
	
		stbi_image_free(data);
	}
}

void Texture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

//////////////////////////////////////////////////////////////////////////////////
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
	impl->window = SDL_CreateWindow(name_.c_str(), x_, y_, width_, height_, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
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

void VideoDevice::drawPoint(const Vector2& v, const Color& c)
{
#ifdef STM32
#else		
	glDisable(GL_TEXTURE_2D);
	
	glBegin(GL_POINTS);

	glColor4f(c.r, c.g, c.b, c.a);
	glVertex2fv((f32*)&v);
	
	glEnd();
#endif
}

void VideoDevice::drawLine(const Vector2& v0, const Color& c0, const Vector2& v1, const Color& c1)
{
#ifdef STM32
#else		
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);

	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glEnd();
#endif
}

void VideoDevice::drawWireTriangle(const Vector2& v0, const Color& c0, const Vector2& v1, const Color& c1, const Vector2& v2, const Color& c2)
{
#ifdef STM32
#else		
	glDisable(GL_TEXTURE_2D);

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

void VideoDevice::drawWireRectangle(const Vector2& v0, const Color& c0, const Vector2& v1, const Color& c1, const Vector2& v2, const Color& c2, const Vector2& v3, const Color& c3)
{
#ifdef STM32
#else		
	glDisable(GL_TEXTURE_2D);

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

void VideoDevice::drawWireCircle(const Vector2& center, f32 radius, const Color& c)
{
}

void VideoDevice::drawSolidTriangle(const Vector2& v0, const Color& c0, const Vector2& v1, const Color& c1, const Vector2& v2, const Color& c2)
{
#ifdef STM32
#else		
	glDisable(GL_TEXTURE_2D);

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

void VideoDevice::drawSolidRectangle(const Vector2& v0, const Color& c0, const Vector2& v1, const Color& c1, const Vector2& v2, const Color& c2, const Vector2& v3, const Color& c3)
{
#ifdef STM32
#else		
	glDisable(GL_TEXTURE_2D);

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

void VideoDevice::drawSolidCircle(const Vector2& center, f32 radius, const Color& c)
{
}

void VideoDevice::drawPrimitive(u32 primitive, const Vector2* vertices, const Color* colors, u32 count)
{
#ifdef STM32
#else
	glDisable(GL_TEXTURE_2D);

	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glColorPointer(4, GL_FLOAT, 0, colors);

	glDrawArrays(primitive, 0, count);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
void VideoDevice::drawTexPoint(const Texture2D& texture, const Vector2& v, const Color& c, const Vector2& t)
{
#ifdef STM32
#else
	glEnable(GL_TEXTURE_2D);
	texture.Bind();

	glTexCoord2fv((f32*)&t);
	glColor4fv((f32*)&c);
	glBegin(GL_POINTS);
	glVertex2fv((f32*)&v);
	glEnd();
#endif
}

void VideoDevice::drawTexLine(const Texture2D& texture, const Vector2& v0, const Color& c0, const Vector2& t0, const Vector2& v1, const Color& c1, const Vector2& t1)
{
#ifdef STM32
#else		
	glEnable(GL_TEXTURE_2D);
	texture.Bind();

	glBegin(GL_LINES);

	glTexCoord2fv((f32*)&t0);
	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glTexCoord2fv((f32*)&t1);
	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glEnd();
#endif
}

void VideoDevice::drawTexWireTriangle(const Texture2D& texture, const Vector2& v0, const Color& c0, const Vector2& t0, const Vector2& v1, const Color& c1, const Vector2& t1, const Vector2& v2, const Color& c2, const Vector2& t2)
{
#ifdef STM32
#else		
	glEnable(GL_TEXTURE_2D);
	texture.Bind();

	glBegin(GL_LINE_LOOP);

	glTexCoord2fv((f32*)&t0);
	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glTexCoord2fv((f32*)&t1);
	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glTexCoord2fv((f32*)&t2);
	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glEnd();
#endif
}

void VideoDevice::drawTexWireRectangle(const Texture2D& texture, 
										const Vector2& v0, const Color& c0, const Vector2& t0, 
										const Vector2& v1, const Color& c1, const Vector2& t1,
										const Vector2& v2, const Color& c2, const Vector2& t2,
										const Vector2& v3, const Color& c3, const Vector2& t3)
{
#ifdef STM32
#else		
	glEnable(GL_TEXTURE_2D);
	texture.Bind();

	glBegin(GL_LINES);

	glTexCoord2fv((f32*)&t0);
	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glTexCoord2fv((f32*)&t1);
	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glTexCoord2fv((f32*)&t2);
	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glTexCoord2fv((f32*)&t3);
	glColor4fv((f32*)&c3);
	glVertex2fv((f32*)&v3);

	glEnd();
#endif
}

void VideoDevice::drawTexSolidTriangle(const Texture2D& texture,
										const Vector2& v0, const Color& c0, const Vector2& t0,
										const Vector2& v1, const Color& c1, const Vector2& t1,
										const Vector2& v2, const Color& c2, const Vector2& t2)
{
#ifdef STM32
#else		
	glEnable(GL_TEXTURE_2D);
	texture.Bind();

	glBegin(GL_TRIANGLES);

	glTexCoord2fv((f32*)&t0);
	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glTexCoord2fv((f32*)&t1);
	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glTexCoord2fv((f32*)&t2);
	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glEnd();
#endif
}


void VideoDevice::drawTexSolidRectangle(const Texture2D& texture,
	const Vector2& v0, const Color& c0, const Vector2& t0,
	const Vector2& v1, const Color& c1, const Vector2& t1,
	const Vector2& v2, const Color& c2, const Vector2& t2,
	const Vector2& v3, const Color& c3, const Vector2& t3)
{
#ifdef STM32
#else		
	glEnable(GL_TEXTURE_2D);
	texture.Bind();

	glBegin(GL_QUADS);

	glTexCoord2fv((f32*)&t0);
	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glTexCoord2fv((f32*)&t1);
	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glTexCoord2fv((f32*)&t2);
	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glTexCoord2fv((f32*)&t3);
	glColor4fv((f32*)&c3);
	glVertex2fv((f32*)&v3);

	glEnd();
#endif
}

void VideoDevice::drawTexPrimitive(const Texture2D& texture, u32 primitive, const Vector2* v, const Color* c, const Vector2* t, u32 count)
{
#ifdef STM32
#else		
	glEnable(GL_TEXTURE_2D);
	texture.Bind();

	glVertexPointer(2, GL_FLOAT, 0, v);
	glColorPointer(4, GL_FLOAT, 0, c);
	glTexCoordPointer(2, GL_FLOAT, 0, t);

	glDrawArrays(primitive, 0, count);
#endif
}