#ifndef _Platform_h_
#define _Platform_h_

#include "vgmdef.h"
using namespace std;

class Platform
{
public:
	static bool initialize();
	static bool update();
	static void terminate();
};

#endif
