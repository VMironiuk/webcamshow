#ifndef VIDEOINPUTBIN_H
#define VIDEOINPUTBIN_H

#include "abstractbin.h"

class VideoInputBin : public AbstractBin
{
public:
    VideoInputBin();
    ~VideoInputBin();
    GstElement *get();
};

#endif // VIDEOINPUTBIN_H
