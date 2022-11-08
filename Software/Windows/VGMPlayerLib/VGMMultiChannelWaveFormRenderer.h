#ifndef _VGMMultiChannelWaveFormRenderer_h_
#define _VGMMultiChannelWaveFormRenderer_h_

#include "VGMData.h"
#include "VGMRenderer.h"
#include "VideoDevice.h"

class VGMMultiChannelWaveFormRenderer : public VGMRenderer
{
public:
	class Skin
	{
	public:
		Skin(UINT8 numChannels_ = 2,
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

		UINT8 numChannels;
		Color bgColor;
		Color gridColor;
		Color axisColor;
		Color leftColor;
		Color rightColor;
	};

	VGMMultiChannelWaveFormRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_, float waveScale_, const VGMMultiChannelWaveFormRenderer::Skin& skin_);
	virtual ~VGMMultiChannelWaveFormRenderer();

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
	float waveScale;
	Skin skin;
	vector<FLOAT32> max;

	Font* font;
};

#endif