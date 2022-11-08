#ifndef _VGMSpectrumRenderer_h_
#define _VGMSpectrumRenderer_h_

#include "VGMData.h"
#include "VGMRenderer.h"
#include "VideoDevice.h"

class VGMSpectrumRenderer : public VGMRenderer
{
public:
	class Skin
	{
	public:
		Skin(UINT8 numColumns_ = 32,
			 UINT8 numChannels_ = 2,
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

		UINT8 numColumns;
		UINT8 numChannels;
		Color bgColor;
		Color gridColor;
		Color axisColor;
		Color leftColor;
		Color rightColor;
	};

	VGMSpectrumRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_, float waveScale_, const VGMSpectrumRenderer::Skin& skin_);
	virtual ~VGMSpectrumRenderer();

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

	vector<FLOAT32> maxLeft;
	vector<FLOAT32> maxRight;

	Font* font;
};

#endif