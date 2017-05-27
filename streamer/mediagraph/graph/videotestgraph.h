#ifndef VIDEOTESTGRAPH_H
#define VIDEOTESTGRAPH_H

#include "abstractgraph.h"

typedef struct _GstElement GstElement;
class VideoTestGraph : public AbstractGraph
{
public:
    VideoTestGraph();
    ~VideoTestGraph();

    bool start();
    void stop();

private:
    void setup();
    void free();
    void cleanup();

    GstElement *m_pipeline;
};

#endif // VIDEOTESTGRAPH_H
