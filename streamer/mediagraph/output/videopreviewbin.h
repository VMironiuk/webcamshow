#ifndef VIDEOPREVIEWBIN_H
#define VIDEOPREVIEWBIN_H

#include "abstractoutputbin.h"

class VideoPreviewBin : public AbstractOutputBin
{
public:
    VideoPreviewBin(int winid);
    ~VideoPreviewBin();
    GstElement *get();

private:
    int m_winid;
};

#endif // VIDEOPREVIEWBIN_H
