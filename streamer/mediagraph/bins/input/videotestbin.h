#ifndef VIDEOTESTBIN_H
#define VIDEOTESTBIN_H

#include "abstractbin.h"

class VideoTestBin : public AbstractBin
{
public:
    VideoTestBin();
    ~VideoTestBin();
    GstElement *get();
};

#endif // VIDEOTESTBIN_H
