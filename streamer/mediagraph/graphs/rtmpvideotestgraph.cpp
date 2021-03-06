#include "rtmpvideotestgraph.h"
#include "videotestbin.h"
#include "audiotestbin.h"
#include "rtmpstreamingbin.h"

#include <gst/gst.h>

RtmpVideoTestGraph::RtmpVideoTestGraph()
    : AbstractGraph(),
      m_pipeline(NULL)
{
    setup();
}

RtmpVideoTestGraph::RtmpVideoTestGraph(const std::string &location)
    : AbstractGraph(),
      m_pipeline(NULL),
      m_location(location)
{
    setup();
}

RtmpVideoTestGraph::~RtmpVideoTestGraph()
{
    free();
}

bool RtmpVideoTestGraph::start()
{
    if (!m_pipeline) {
        g_printerr("RtmpVideoTestGraph: ERROR: failed to start streaming, pipeline is corrupted\n");
        return FALSE;
    }

    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

    return TRUE;
}

void RtmpVideoTestGraph::stop()
{
    if (!m_pipeline) {
        g_printerr("RtmpVideoTestGraph: ERROR: failed to stop streaming, pipeline is corrupted\n");
        return;
    }

    gst_element_set_state(m_pipeline, GST_STATE_NULL);
}

void RtmpVideoTestGraph::setup()
{
    // Create elements
    m_pipeline = gst_pipeline_new("pipeline");
    if (!m_pipeline) {
        g_printerr("RtmpVideoTestGraph: ERROR: failed to create element of type 'pipeline'\n");
        return;
    }

    VideoTestBin videotestbin;
    GstElement *videosrc = videotestbin.get();
    if (!videosrc) {
        g_printerr("RtmpVideoTestGraph: ERROR: failed to create element of type 'videotestbin'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), videosrc)) {
        g_printerr("RtmpVideoTestGraph: ERROR: bin doesn't want to accept element of type 'videotestbin'\n");
        gst_object_unref(GST_OBJECT(videosrc));
        free();
        return;
    }

    AudioTestBin audiotestbin;
    GstElement *audiosrc = audiotestbin.get();
    if (!audiosrc) {
        g_printerr("RtmpVideoTestGraph: ERROR: failed to create element of type 'audiotestbin'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), audiosrc)) {
        g_printerr("RtmpVideoTestGraph: ERROR: bin doesn't want to accept element of type 'audiotestbin'\n");
        gst_object_unref(GST_OBJECT(audiosrc));
        free();
        return;
    }

    RtmpStreamingBin rtmpstreamingbin(m_location);
    GstElement *rtmpbin = rtmpstreamingbin.get();
    if (!rtmpbin) {
        g_printerr("RtmpVideoTestGraph: ERROR: failed to create element of type 'rtmpbin'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), rtmpbin)) {
        g_printerr("RtmpVideoTestGraph: ERROR: bin doesn't want to accept element of type 'rtmpbin'\n");
        gst_object_unref(GST_OBJECT(rtmpbin));
        free();
        return;
    }

    // Link elements
    if (!gst_element_link_pads(videosrc, VideoTestBin::GHOST_PAD_NAME, rtmpbin, RtmpStreamingBin::VIDEO_GHOST_PAD_NAME)) {
        g_printerr("RtmpVideoTestGraph: ERROR: cannot link elements of types 'videotestbin' and 'rtmpbin'\n");
        free();
        return;
    }

    if (!gst_element_link_pads(audiosrc, AudioTestBin::GHOST_PAD_NAME, rtmpbin, RtmpStreamingBin::AUDIO_GHOST_PAD_NAME)) {
        g_printerr("RtmpGraph: ERROR: cannot link elements of types 'audiotestbin' and 'rtmpbin'\n");
        free();
        return;
    }
}

void RtmpVideoTestGraph::free()
{
    if (m_pipeline) {
        gst_object_unref(GST_OBJECT(m_pipeline));
        m_pipeline = NULL;
    }
}
