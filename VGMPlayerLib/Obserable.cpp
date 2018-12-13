#include "Obserable.h"
using namespace std;

Obserable::Obserable()
{
}

Obserable::~Obserable()
{
}

void Obserable::attach(Observer& observer)
{
	observers.push_back(&observer);
}

void Obserable::detach(Observer& observer)
{
	observers.remove(&observer);
}

void Obserable::notifySomething()
{
	for (Observer *o : observers)
	{
		o->onNotifySomething(*this);
	}
}

void Obserable::notifyOpen()
{
	for (Observer *o : observers)
	{
		o->onNotifyOpen(*this);
	}
}

void Obserable::notifyClose()
{
	for (Observer *o : observers)
	{
		o->onNotifyClose(*this);
	}
}

void Obserable::notifyPlay()
{
	for (Observer *o : observers)
	{
		o->onNotifyPlay(*this);
	}
}

void Obserable::notifyStop()
{
	for (Observer *o : observers)
	{
		o->onNotifyStop(*this);
	}
}

void Obserable::notifyPause()
{
	for (Observer *o : observers)
	{
		o->onNotifyPause(*this);
	}
}

void Obserable::notifyResume()
{
	for (Observer *o : observers)
	{
		o->onNotifyResume(*this);
	}
}

void Obserable::notifyUpdate()
{
	for (Observer *o : observers)
	{
		o->onNotifyUpdate(*this);
	}
}