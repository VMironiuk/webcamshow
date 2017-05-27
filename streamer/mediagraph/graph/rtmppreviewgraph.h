#ifndef RTMPPREVIEWGRAPH_H
#define RTMPPREVIEWGRAPH_H

#include "abstractgraph.h"

#include <string>
#include <vector>

typedef struct _GstElement GstElement;
typedef struct _GstPad GstPad;
class RtmpPreviewGraph : public AbstractGraph
{
public:
    RtmpPreviewGraph();
    RtmpPreviewGraph(int winid, const std::string &location);
    ~RtmpPreviewGraph();

    bool start();
    void stop();

private:
    void setup();
    void free();

    GstElement *m_pipeline;
    int m_winId;
    std::string m_location;
    std::vector<GstPad*> m_requested_pads;
};

#endif // RTMPPREVIEWGRAPH_H
