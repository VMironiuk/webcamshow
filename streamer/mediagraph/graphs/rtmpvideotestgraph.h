#ifndef RTMPVIDEOTESTGRAPH_H
#define RTMPVIDEOTESTGRAPH_H

#include "abstractgraph.h"

#include <string>

typedef struct _GstElement GstElement;
class RtmpVideoTestGraph : public AbstractGraph
{
public:
    RtmpVideoTestGraph();
    RtmpVideoTestGraph(const std::string &location);
    ~RtmpVideoTestGraph();

    bool start();
    void stop();

private:
    void setup();
    void free();

    GstElement *m_pipeline;
    std::string m_location;
};

#endif // RTMPVIDEOTESTGRAPH_H
