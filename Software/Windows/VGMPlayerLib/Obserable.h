#ifndef _Obserable_h_
#define _Obserable_h_

#include "vgmdef.h"
#include "Observer.h"
#include <list>
using namespace std;

class Obserable
{
public:
	Obserable();
	~Obserable();

	void Attach(Observer& observer);
	void Detach(Observer& observer);
protected:
	void NotifySomething();
	void NotifyOpen();
	void NotifyClose();
	void NotifyPlay();
	void NotifyStop();
	void NotifyPause();
	void NotifyResume();
	void NotifyUpdate(bool needUpdateSample);
private:
	list<Observer *> observers;
};

#endif
