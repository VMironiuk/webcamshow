#ifndef VIDEOINPUTBIN_H
#define VIDEOINPUTBIN_H

#include "abstractbin.h"

class VideoInputBin : public AbstractBin
{
public:
    VideoInputBin();
    ~VideoInputBin();
    GstElement *get();

    static const char *BIN_NAME;
    static const char *SRC_NAME;
    static const char *SRC_ALIAS;
    static const char *GHOST_PAD_NAME;
};

#endif // VIDEOINPUTBIN_H
