#include "streamer.h"
#include "previewgraph.h"
#include "rtmpgraph.h"
#include "rtmppreviewgraph.h"
#include "videotestgraph.h"
#include "rtmpvideotestgraph.h"
#include "rtmppreviewvideotestgraph.h"

#include <unistd.h>
#include <iostream>
#include <string>

enum StreamType
{
    Default = 0,
    Preview = 1 << 0,
    Rtmp = 1 << 1,
    RtmpPreview = Preview | Rtmp,
    VideoTest =  1 << 2,
    RtmpVideoTest = VideoTest | Rtmp,
    RtmpPreviewVideoTest = VideoTest | Preview | Rtmp
};

static bool waitForInput()
{
    char input;
    bool stop = false;

    while (!stop) {
        std::cin >> input;

        switch (input) {
        case 'q':
            stop = true;
            break;
        default:
            break;
        }
    }
    return stop;
}

Streamer::Streamer(int argc, char **argv)
    : m_graph(nullptr)
{
    char c;
    int winid = 0;
    std::string rtmp_location;

    unsigned stream_type = Default;

    while ((c = getopt(argc, argv, "r:p:t")) != EOF) {
        switch (c) {
        case 'p':
            winid = std::atoi(optarg);
            stream_type |= Preview;
            break;

        case 'r':
            rtmp_location = optarg;
            stream_type |= Rtmp;
            break;

        case 't':
            stream_type |= VideoTest;
            break;

        default:
            break;
        }
    }

    switch (stream_type) {
    case Preview:
        m_graph = new PreviewGraph(winid);
        break;

    case Rtmp:
        m_graph = new RtmpGraph(rtmp_location);
        break;

    case RtmpPreview:
        m_graph = new RtmpPreviewGraph(winid, rtmp_location);
        break;

    case VideoTest:
        m_graph = new VideoTestGraph(winid);
        break;

    case RtmpVideoTest:
        m_graph = new RtmpVideoTestGraph(rtmp_location);
        break;

    case RtmpPreviewVideoTest:
        m_graph = new RtmpPreviewVideoTestGraph(winid, rtmp_location);
        break;

    case Default: // fall down
    default:
        m_graph = new VideoTestGraph(winid);
        break;
    }
}

Streamer::~Streamer()
{
    if (m_graph)
        delete m_graph;
}

int Streamer::exec()
{
    if (!m_graph) {
        std::cout << "ERROR: graph is corrupted" << std::endl;
        return 1;
    }

    if (!m_graph->start()) {
        std::cout << "ERROR: failed to start streaming, pipeline is corrupted" << std::endl;
        return 1;
    }

    while (!waitForInput()) { /* empty */ }

    m_graph->stop();

    return 0;
}
