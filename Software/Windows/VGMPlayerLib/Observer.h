#ifndef _Observer_h_
#define _Observer_h_

class Obserable;
class Observer
{
	friend class Obserable;
public:
	Observer();
	~Observer();
protected:
	virtual void OnNotifySomething(Obserable& obserable) = 0;
	virtual void OnNotifyOpen(Obserable& obserable) = 0;
	virtual void OnNotifyClose(Obserable& obserable) = 0;
	virtual void OnNotifyPlay(Obserable& obserable) = 0;
	virtual void OnNotifyStop(Obserable& obserable) = 0;
	virtual void OnNotifyPause(Obserable& obserable) = 0;
	virtual void OnNotifyResume(Obserable& obserable) = 0;
	virtual void OnNotifyUpdate(Obserable& obserable, bool needUpdateSample) = 0;
private:
public:
protected:
private:
};

#endif
