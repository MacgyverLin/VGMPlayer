#ifndef _VGMDataObverser_h_
#define _VGMDataObverser_h_

#include "Observer.h"
#include "VGMData.h"

class VGMDataObverser : public Observer
{
public:
	VGMDataObverser(VGMData& vgmData);
	virtual ~VGMDataObverser();
protected:
private:

public:
protected:
private:
	VGMData& vgmData;
};

#endif