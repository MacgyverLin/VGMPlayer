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
	list<Observer *>::iterator itr;
	for(itr=observers.begin(); itr!=observers.end(); itr++)
	{
		(*itr)->onNotifySomething(*this);
	}
}

void Obserable::notifyOpen()
{
	list<Observer *>::iterator itr;
	for(itr=observers.begin(); itr!=observers.end(); itr++)
	{
		(*itr)->onNotifyOpen(*this);
	}
}

void Obserable::notifyClose()
{
	list<Observer *>::iterator itr;
	for(itr=observers.begin(); itr!=observers.end(); itr++)
	{
		(*itr)->onNotifyClose(*this);
	}
}

void Obserable::notifyPlay()
{
	list<Observer *>::iterator itr;
	for(itr=observers.begin(); itr!=observers.end(); itr++)
	{
		(*itr)->onNotifyPlay(*this);
	}
}

void Obserable::notifyStop()
{
	list<Observer *>::iterator itr;
	for(itr=observers.begin(); itr!=observers.end(); itr++)
	{
		(*itr)->onNotifyStop(*this);
	}
}

void Obserable::notifyPause()
{
	list<Observer *>::iterator itr;
	for(itr=observers.begin(); itr!=observers.end(); itr++)
	{
		(*itr)->onNotifyPause(*this);
	}
}

void Obserable::notifyResume()
{
	list<Observer *>::iterator itr;
	for(itr=observers.begin(); itr!=observers.end(); itr++)
	{
		(*itr)->onNotifyResume(*this);
	}
}

void Obserable::notifyUpdate()
{
	list<Observer *>::iterator itr;
	for(itr=observers.begin(); itr!=observers.end(); itr++)
	{
		(*itr)->onNotifyUpdate(*this);
	}
}
