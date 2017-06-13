#ifndef VIDEOPREVIEWBIN_H
#define VIDEOPREVIEWBIN_H

#include "abstractbin.h"

class VideoPreviewBin : public AbstractBin
{
public:
    VideoPreviewBin(int winid);
    ~VideoPreviewBin();
    GstElement *get();

    static const char *BIN_NAME;
    static const char *QUEUE_NAME;
    static const char *QUEUE_ALIAS;
    static const char *GHOST_PAD_NAME;

private:
    int m_winid;
};

#endif // VIDEOPREVIEWBIN_H
