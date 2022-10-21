#ifndef _Platform_h_
#define _Platform_h_

#include "vgmdef.h"
using namespace std;

class Platform
{
public:
	static bool Initialize();
	static bool Update();
	static bool getKeyDown(int key);
	static bool getKeyUp(int key);
	static bool getKey(int key);
	static void Terminate();
};

#endif
