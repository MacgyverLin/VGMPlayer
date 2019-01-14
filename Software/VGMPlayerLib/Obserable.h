#ifndef _Obserable_h_
#define _Obserable_h_

#include "vgmdef.h"
#include "Observer.h"
#include <Array.h>

using namespace std;

class Obserable
{
public:
	Obserable();
	~Obserable();

	void attach(Observer& observer);
	void detach(Observer& observer);
protected:
	void notifySomething();
	void notifyOpen();
	void notifyClose();
	void notifyPlay();
	void notifyStop();
	void notifyPause();
	void notifyResume();
	void notifyUpdate();
private:
	Vector<Observer *> observers;
};

#endif
