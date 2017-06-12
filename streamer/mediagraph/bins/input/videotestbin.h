#ifndef VIDEOTESTBIN_H
#define VIDEOTESTBIN_H

#include "abstractbin.h"

class VideoTestBin : public AbstractBin
{
public:
    VideoTestBin();
    ~VideoTestBin();
    GstElement *get();

    static const char *BIN_NAME;
    static const char *SRC_NAME;
    static const char *SRC_ALIAS;
    static const char *GHOST_PAD_NAME;
};

#endif // VIDEOTESTBIN_H
