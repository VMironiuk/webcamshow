#ifndef VIDEOPREVIEWBIN_H
#define VIDEOPREVIEWBIN_H

#include "abstractbin.h"

class VideoPreviewBin : public AbstractBin
{
public:
    VideoPreviewBin(int winid);
    ~VideoPreviewBin();
    GstElement *get();

private:
    int m_winid;
};

#endif // VIDEOPREVIEWBIN_H
