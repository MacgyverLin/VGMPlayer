#ifndef __VIDEO_ENCODER_H__
#define __VIDEO_ENCODER_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>

class VideoEncoderImpl;

class VideoEncoder
{
public:
    VideoEncoder();
    virtual ~VideoEncoder();

    bool Initiate(const char* filename);
    bool AddFrame(const char* imageBuffer, int imageBufferSize,
                  const char* soundBuffer, int soundBufferSize);
    void Terminate();
private:
    VideoEncoderImpl* impl;
};

#endif // __VIDEO_ENCODER_H__