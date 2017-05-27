#ifndef RTMPGRAPH_H
#define RTMPGRAPH_H

#include "abstractgraph.h"

#include <string>

typedef struct _GstElement GstElement;
class RtmpGraph : public AbstractGraph
{
public:
    RtmpGraph();
    RtmpGraph(const std::string &location);
    ~RtmpGraph();

    bool start();
    void stop();

private:
    void setup();
    void free();

    GstElement *m_pipeline;
    std::string m_location;
};

#endif // RTMPGRAPH_H
