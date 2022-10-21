#include "Obserable.h"

Obserable::Obserable()
{
}

Obserable::~Obserable()
{
}

void Obserable::Attach(Observer& observer)
{
	observers.push() = &observer;
}

void Obserable::Detach(Observer& observer)
{
	//observers.remove(&observer);
}

void Obserable::NotifySomething()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->OnNotifySomething(*this);
	}
}

void Obserable::NotifyOpen()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->OnNotifyOpen(*this);
	}
}

void Obserable::NotifyClose()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->OnNotifyClose(*this);
	}
}

void Obserable::NotifyPlay()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->OnNotifyPlay(*this);
	}
}

void Obserable::NotifyStop()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->OnNotifyStop(*this);
	}
}

void Obserable::NotifyPause()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->OnNotifyPause(*this);
	}
}

void Obserable::NotifyResume()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->OnNotifyResume(*this);
	}
}

void Obserable::NotifyUpdate()
{
	for(u32 i=0; i<observers.size(); i++)
	{
		observers[i]->OnNotifyUpdate(*this);
	}
}
