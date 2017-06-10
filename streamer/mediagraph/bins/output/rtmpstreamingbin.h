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

private:
    std::string m_location;
};

#endif // RTMPSTREAMINGBIN_H
