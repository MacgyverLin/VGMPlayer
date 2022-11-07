#ifndef _VGMMultiChannelNoteViewer_h_
#define _VGMMultiChannelNoteViewer_h_

#include "VGMData.h"
#include "VGMRenderer.h"
#include "VideoDevice.h"
#include <list>

class VGMMultiChannelNoteRenderer : public VGMRenderer
{
public:
	class Skin
	{
	public:
		Skin(u8 numChannels_ = 2,
			const Color& bgColor_ = Color(0.3f, 0.3f, 0.3f, 1.0f), const Color& gridColor_ = Color(0.0f, 0.2f, 0.2f, 1.0f), const Color& axisColor_ = Color(0.3f, 0.3f, 0.3f, 1.0f),
			const Color& leftColor_ = Color(0.3f, 1.0f, 0.3f, 1.0f), const Color& rightColor_ = Color(0.3f, 1.0f, 1.0f, 1.0f))
			: numChannels(numChannels_)
			, bgColor(bgColor_)
			, gridColor(gridColor_)
			, axisColor(axisColor_)
			, leftColor(leftColor_)
			, rightColor(rightColor_)
		{
		}

		u8 numChannels;
		Color bgColor;
		Color gridColor;
		Color axisColor;
		Color leftColor;
		Color rightColor;
	};

	VGMMultiChannelNoteRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_, float waveScale_, const VGMMultiChannelNoteRenderer::Skin& skin_);
	virtual ~VGMMultiChannelNoteRenderer();

	virtual void OnNotifySomething(Obserable& vgmData) override;
	virtual void OnNotifyOpen(Obserable& vgmData) override;
	virtual void OnNotifyClose(Obserable& vgmData) override;
	virtual void OnNotifyPlay(Obserable& vgmData) override;
	virtual void OnNotifyStop(Obserable& vgmData) override;
	virtual void OnNotifyPause(Obserable& vgmData) override;
	virtual void OnNotifyResume(Obserable& vgmData) override;
	virtual void OnNotifyUpdate(Obserable& vgmData) override;
private:
public:
protected:
private:
	float waveScale;
	Skin skin;
	std::list<string> commandStrings;

	Font* font;

	vector<f32> maxLeft;
	vector<f32> maxRight;

	string titleString;
	Texture2D logo;
};

#endif