#ifndef _Obserable_h_
#define _Obserable_h_

#include "vgmdef.h"
#include <string>
#include <list>
#include "Observer.h"
using namespace std;

class Obserable
{
public:
	Obserable();
	virtual ~Obserable();

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
	list<Observer *> observers;
};

#endif