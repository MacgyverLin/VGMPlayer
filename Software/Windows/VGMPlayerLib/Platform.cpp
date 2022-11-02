#include "Platform.h"

#include <stdio.h>
#include <SDL_main.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <GL/gl.h>

bool Platform::Initialize()
{
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
}

static bool oldkeyStates[256] = {};
static bool keyStates[256] = {};

int Platform::GetScreenWidth()
{
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);

	return DM.w;
}

int Platform::GetScreenHeight()
{
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);

	return DM.h;
}

bool Platform::GetKeyDown(int key)
{
	return !oldkeyStates[key] && keyStates[key];
}

bool Platform::GetKeyUp(int key)
{
	return oldkeyStates[key] && !keyStates[key];
}

bool Platform::GetKey(int key)
{
	return oldkeyStates[key] && keyStates[key];
}

bool Platform::Update()
{
	SDL_Event sdlEvent;

	while (SDL_PollEvent(&sdlEvent) != 0)
	{
		// Esc button is pressed
		if (sdlEvent.type == SDL_QUIT)
		{
			return false;
		}
	}


	const Uint8* state = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < 256; i++)
	{
		oldkeyStates[i] = keyStates[i];
		keyStates[i] = state[i];
	}
	
	return true;
}

void Platform::Terminate()
{
	SDL_Quit();
}
