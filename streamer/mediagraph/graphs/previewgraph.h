#ifndef PREVIEWGRAPH_H
#define PREVIEWGRAPH_H

#include "abstractgraph.h"

typedef struct _GstElement GstElement;
class PreviewGraph : public AbstractGraph
{
public:
    PreviewGraph();
    PreviewGraph(int winId);
    ~PreviewGraph();

    bool start();
    void stop();

private:
    void setup();
    void free();

    GstElement *m_pipeline;
    int m_winId;
};

#endif // PREVIEWGRAPH_H
