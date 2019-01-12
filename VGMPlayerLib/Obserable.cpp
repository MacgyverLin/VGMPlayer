#include "Obserable.h"

Obserable::Obserable()
{
}

Obserable::~Obserable()
{
}

void Obserable::attach(Observer& observer)
{
	observers.push() = &observer;
}

void Obserable::detach(Observer& observer)
{
	//observers.remove(&observer);
}

void Obserable::notifySomething()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->onNotifySomething(*this);
	}
}

void Obserable::notifyOpen()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->onNotifyOpen(*this);
	}
}

void Obserable::notifyClose()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->onNotifyClose(*this);
	}
}

void Obserable::notifyPlay()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->onNotifyPlay(*this);
	}
}

void Obserable::notifyStop()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->onNotifyStop(*this);
	}
}

void Obserable::notifyPause()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->onNotifyPause(*this);
	}
}

void Obserable::notifyResume()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->onNotifyResume(*this);
	}
}

void Obserable::notifyUpdate()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->onNotifyUpdate(*this);
	}
}
