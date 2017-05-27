#ifndef YOUTUBEWITHPREVIEWGRAPH_H
#define YOUTUBEWITHPREVIEWGRAPH_H

#include "abstractgraph.h"

#include <string>

typedef struct _GstElement GstElement;
class RtmpPreviewGraph : public AbstractGraph
{
public:
    RtmpPreviewGraph();
    RtmpPreviewGraph(int winid, const std::string &location = "");
    ~RtmpPreviewGraph();

    bool start();
    void stop();

private:
    void setup();
    void free();
    void cleanup();

    GstElement *m_pipeline;
    int m_winId;
    std::string m_location;
};

#endif // YOUTUBEWITHPREVIEWGRAPH_H
