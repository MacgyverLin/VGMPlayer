#ifndef _VGMMultiChannelNoteViewer_h_
#define _VGMMultiChannelNoteViewer_h_

#include "VGMData.h"
#include "VGMDataObverser.h"
#include "VideoDevice.h"

class VGMMultiChannelNoteViewer : public VGMDataObverser
{
public:
	class Skin
	{
	public:
		Skin(u8 numColumns_ = 16,
			u8 numChannels_ = 2,
			const Color& bgColor_ = Color(0.3f, 0.3f, 0.3f, 1.0f), const Color& gridColor_ = Color(0.0f, 0.2f, 0.2f, 1.0f), const Color& axisColor_ = Color(0.3f, 0.3f, 0.3f, 1.0f),
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
	VGMMultiChannelNoteViewer(const string& name_, u32 x_, u32 y_, u32 width_, u32 height_, float waveScale_, const Skin& skin_);
	virtual ~VGMMultiChannelNoteViewer();
protected:
	virtual void OnNotifySomething(Obserable& vgmData);
	virtual void OnNotifyOpen(Obserable& vgmData);
	virtual void OnNotifyClose(Obserable& vgmData);
	virtual void OnNotifyPlay(Obserable& vgmData);
	virtual void OnNotifyStop(Obserable& vgmData);
	virtual void OnNotifyPause(Obserable& vgmData);
	virtual void OnNotifyResume(Obserable& vgmData);
	virtual void OnNotifyUpdate(Obserable& vgmData);
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
	float waveScale;
	Skin skin;

	vector<f32> maxLeft;
	vector<f32> maxRight;

	Texture2D texture;

	std::vector<std::vector<VGMData::Channel::Note>> channelsNotes;
};

#endif