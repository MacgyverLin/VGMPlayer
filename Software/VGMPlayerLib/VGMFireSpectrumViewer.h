#ifndef _VGMFireSpectrumViewer_h_
#define _VGMFireSpectrumViewer_h_

#include "VGMData.h"
#include "VGMDataObverser.h"
#include "VideoDevice.h"

class VGMFireSpectrumViewer : public VGMDataObverser
{
public:
	class Skin
	{
	public:
		Skin(u8 numColumns_ = 128,
			 u8 numChannels_ = 2,
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

		u8 numColumns;
		u8 numChannels;
		Color bgColor;
		Color gridColor;
		Color axisColor;
		Color leftColor;
		Color rightColor;
	};
	VGMFireSpectrumViewer(const string& name_, u32 x_, u32 y_, u32 width_, u32 height_, 
					   const Skin& skin_);
	virtual ~VGMFireSpectrumViewer();
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
	u32 x;
	u32 y;
	u32 width;
	u32 height;
	Skin skin;

	vector<f32> maxLeft;
	vector<f32> maxRight;
};

#endif