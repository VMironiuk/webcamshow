#ifndef VIDEOTESTGRAPH_H
#define VIDEOTESTGRAPH_H

#include "abstractgraph.h"

typedef struct _GstElement GstElement;
class VideoTestGraph : public AbstractGraph
{
public:
    VideoTestGraph();
    VideoTestGraph(int winId);
    ~VideoTestGraph();

    bool start();
    void stop();

private:
    void setup();
    void free();

    GstElement *m_pipeline;
    int m_winId;
};

#endif // VIDEOTESTGRAPH_H
