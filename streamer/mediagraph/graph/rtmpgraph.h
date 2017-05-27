#ifndef RTMPGRAPH_H
#define RTMPGRAPH_H

#include "abstractgraph.h"

#include <string>

typedef struct _GstElement GstElement;
class RtmpGraph : public AbstractGraph
{
public:
    RtmpGraph();
    RtmpGraph(const std::string &rtmp_location = "");
    ~RtmpGraph();

    bool start();
    void stop();

private:
    void setup();
    void free();
    void cleanup();

    GstElement *m_pipeline;
    std::string m_rtmp_location;
};

#endif // RTMPGRAPH_H
