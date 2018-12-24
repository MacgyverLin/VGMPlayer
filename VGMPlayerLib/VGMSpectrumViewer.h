#ifndef _VGMSpectrumViewer_h_
#define _VGMSpectrumViewer_h_

#include "VGMData.h"
#include "VGMDataObverser.h"
#include "VideoDevice.h"

class VGMSpectrumViewer : public VGMDataObverser
{
public:
	class Skin
	{
	public:
		Skin(UINT8 numColumns_ = 16,
			 UINT8 numChannels_ = 2,
			const Color& bgColor_ = Color(0.0f, 0.0f, 0.0f, 1.0f), const Color& gridColor_ = Color(0.0f, 0.2f, 0.2f, 1.0f), const Color& axisColor_ = Color(0.9f, 0.9f, 0.9f, 1.0f),
			const Color& leftColor_ = Color(0.3f, 1.0f, 0.3f, 1.0f), const Color& rightColor_ = Color(0.3f, 1.0f, 1.0f, 1.0f))
			: numColumns(numColumns_)
			, numChannels(numChannels_)
			, bgColor(bgColor_)
			, gridColor(gridColor_)
			, axisColor(axisColor_)
			, leftColor(leftColor_)
			, rightColor(rightColor_)
		{
		}

		UINT8 numColumns;
		UINT8 numChannels;
		Color bgColor;
		Color gridColor;
		Color axisColor;
		Color leftColor;
		Color rightColor;
	};
	VGMSpectrumViewer(const string& name_, UINT32 x_, UINT32 y_, UINT32 width_, UINT32 height_, 
					   const Skin& skin_);
	virtual ~VGMSpectrumViewer();
protected:
	virtual void onNotifySomething(Obserable& vgmData);
	virtual void onNotifyOpen(Obserable& vgmData);
	virtual void onNotifyClose(Obserable& vgmData);
	virtual void onNotifyPlay(Obserable& vgmData);
	virtual void onNotifyStop(Obserable& vgmData);
	virtual void onNotifyPause(Obserable& vgmData);
	virtual void onNotifyResume(Obserable& vgmData);
	virtual void onNotifyUpdate(Obserable& vgmData);
private:
public:
protected:
private:
	VideoDevice videoDevice;

	string name;
	UINT32 x;
	UINT32 y;
	UINT32 width;
	UINT32 height;
	Skin skin;

	vector<FLOAT32> maxLeft;
	vector<FLOAT32> maxRight;
};

#endif