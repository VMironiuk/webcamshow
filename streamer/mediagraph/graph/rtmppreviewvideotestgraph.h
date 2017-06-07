#ifndef RTMPPREVIEWVIDEOTESTGRAPH_H
#define RTMPPREVIEWVIDEOTESTGRAPH_H

#include "abstractgraph.h"

#include <string>
#include <vector>

typedef struct _GstElement GstElement;
typedef struct _GstPad GstPad;
class RtmpPreviewVideoTestGraph : public AbstractGraph
{
public:
    RtmpPreviewVideoTestGraph();
    RtmpPreviewVideoTestGraph(int winid, const std::string &location);
    ~RtmpPreviewVideoTestGraph();

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

#endif // RTMPPREVIEWVIDEOTESTGRAPH_H
