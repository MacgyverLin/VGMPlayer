#ifndef __VIDEO_ENCODER_H__
#define __VIDEO_ENCODER_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include "Array.h"
#include <vector>

class VideoEncoderImpl;

class VideoEncoder
{
public:
    VideoEncoder();
    virtual ~VideoEncoder();

    bool Initiate(const char* filename, int width, int height);
    bool Update(const Vector<unsigned char>& videoBuffer, const std::vector<INT16>& audioBuffer);
    void Terminate();
private:
    VideoEncoderImpl* impl;
};

#endif // __VIDEO_ENCODER_H__