#ifndef _Observer_h_
#define _Observer_h_

class Obserable;
class Observer
{
	friend class Obserable;
public:
	Observer();
	virtual ~Observer();
protected:
	virtual void onNotifySomething(Obserable& obserable) = 0;
	virtual void onNotifyOpen(Obserable& obserable) = 0;
	virtual void onNotifyClose(Obserable& obserable) = 0;
	virtual void onNotifyPlay(Obserable& obserable) = 0;
	virtual void onNotifyStop(Obserable& obserable) = 0;
	virtual void onNotifyPause(Obserable& obserable) = 0;
	virtual void onNotifyResume(Obserable& obserable) = 0;
	virtual void onNotifyUpdate(Obserable& obserable) = 0;
private:
public:
protected:
private:
};

#endif