#ifndef RTMPSTREAMINGBIN_H
#define RTMPSTREAMINGBIN_H

#include "abstractbin.h"

#include <string>

class RtmpStreamingBin : public AbstractBin
{
public:
    RtmpStreamingBin(const std::string &location);
    ~RtmpStreamingBin();
    GstElement *get();

    static const char *BIN_NAME;
    static const char *MUXER_NAME;
    static const char *MUXER_ALIAS;
    static const char *SINK_NAME;
    static const char *SINK_ALIAS;
    static const char *QUEUE_NAME;
    static const char *AUDIO_QUEUE_ALIAS;
    static const char *VIDEO_QUEUE_ALIAS;
    static const char *FAAC_NAME;
    static const char *FAAC_ALIAS;
    static const char *AUDIO_GHOST_PAD_NAME;
    static const char *H264_NAME;
    static const char *H264_ALIAS;
    static const char *VIDEO_GHOST_PAD_NAME;

private:
    std::string m_location;
};

#endif // RTMPSTREAMINGBIN_H
