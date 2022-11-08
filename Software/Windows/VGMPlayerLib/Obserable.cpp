#include "Obserable.h"

Obserable::Obserable()
{
}

Obserable::~Obserable()
{
}

void Obserable::Attach(Observer& observer)
{
	observers.push_back(&observer);
}

void Obserable::Detach(Observer& observer)
{
	observers.remove(&observer);
}

void Obserable::NotifySomething()
{
	for(auto& observer : observers)
	{
		observer->OnNotifySomething(*this);
	}
}

void Obserable::NotifyOpen()
{
	for (auto& observer : observers)
	{
		observer->OnNotifyOpen(*this);
	}
}

void Obserable::NotifyClose()
{
	for (auto& observer : observers)
	{
		observer->OnNotifyClose(*this);
	}
}

void Obserable::NotifyPlay()
{
	for (auto& observer : observers)
	{
		observer->OnNotifyPlay(*this);
	}
}

void Obserable::NotifyStop()
{
	for (auto& observer : observers)
	{
		observer->OnNotifyStop(*this);
	}
}

void Obserable::NotifyPause()
{
	for (auto& observer : observers)
	{
		observer->OnNotifyPause(*this);
	}
}

void Obserable::NotifyResume()
{
	for (auto& observer : observers)
	{
		observer->OnNotifyResume(*this);
	}
}

void Obserable::NotifyUpdate(bool needUpdateSample)
{
	for (auto& observer : observers)
	{
		observer->OnNotifyUpdate(*this, needUpdateSample);
	}
}
