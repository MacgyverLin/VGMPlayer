#include "Platform.h"

#ifdef STM32
#include <VGMBoard.h>
#else
#include <stdio.h>
#include <SDL_main.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

bool Platform::initialize()
{
#ifdef STM32
	return VGMBoard_Initialize(0, 0);
#else
	//Use OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Initialize video subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		// Display error message
		//printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	return true;
#endif
}

bool Platform::update()
{
#ifdef STM32
	VGMBoard_Update();
	
	return true;
#else	
	SDL_Event sdlEvent;

	while (SDL_PollEvent(&sdlEvent) != 0)
	{
		// Esc button is pressed
		if (sdlEvent.type == SDL_QUIT)
		{
			return false;
		}
	}
	
	return true;
#endif
}

void Platform::terminate()
{
#ifdef STM32
	VGMBoard_Shutdown();
#else	
	SDL_Quit();
#endif
}
