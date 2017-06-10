#include "rtmpgraph.h"
#include "videoinputbin.h"
#include "audioinputbin.h"
#include "rtmpstreamingbin.h"

#include <gst/gst.h>

RtmpGraph::RtmpGraph()
    : AbstractGraph(),
      m_pipeline(NULL)
{
    setup();
}

RtmpGraph::RtmpGraph(const std::string &location)
    : AbstractGraph(),
      m_pipeline(NULL),
      m_location(location)
{
    setup();
}

RtmpGraph::~RtmpGraph()
{
    free();
}

bool RtmpGraph::start()
{
    if (!m_pipeline) {
        g_printerr("RtmpGraph: ERROR: failed to start streaming, pipeline is corrupted\n");
        return FALSE;
    }

    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

    return TRUE;
}

void RtmpGraph::stop()
{
    if (!m_pipeline) {
        g_printerr("RtmpGraph: ERROR: failed to stop streaming, pipeline is corrupted\n");
        return;
    }

    gst_element_set_state(m_pipeline, GST_STATE_NULL);
}

void RtmpGraph::setup()
{
    // Create elements
    m_pipeline = gst_pipeline_new("pipeline");
    if (!m_pipeline) {
        g_printerr("RtmpGraph: ERROR: failed to create element of type 'pipeline'\n");
        return;
    }

    VideoInputBin videoinputbin;
    GstElement *videosrc = videoinputbin.get();
    if (!videosrc) {
        g_printerr("RtmpGraph: ERROR: failed to create element of type 'videoinputbin'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), videosrc)) {
        g_printerr("RtmpGraph: ERROR: bin doesn't want to accept element of type 'videoinputbin'\n");
        gst_object_unref(GST_OBJECT(videosrc));
        free();
        return;
    }

    AudioInputBin audioinputbin;
    GstElement *audiosrc = audioinputbin.get();
    if (!audiosrc) {
        g_printerr("RtmpGraph: ERROR: failed to create element of type 'audioinputbin'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), audiosrc)) {
        g_printerr("RtmpGraph: ERROR: bin doesn't want to accept element of type 'audioinputbin'\n");
        gst_object_unref(GST_OBJECT(audiosrc));
        free();
        return;
    }

    RtmpStreamingBin rtmpstreamingbin(m_location);
    GstElement *rtmpbin = rtmpstreamingbin.get();
    if (!rtmpbin) {
        g_printerr("RtmpGraph: ERROR: failed to create element of type 'rtmpbin'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), rtmpbin)) {
        g_printerr("RtmpGraph: ERROR: bin doesn't want to accept element of type 'rtmpbin'\n");
        gst_object_unref(GST_OBJECT(rtmpbin));
        free();
        return;
    }

    // Link elements
    if (!gst_element_link_pads(videosrc, "src", rtmpbin, "videosink")) {
        g_printerr("RtmpGraph: ERROR: cannot link elements of types 'videoinputbin' and 'rtmpbin'\n");
        free();
        return;
    }

    if (!gst_element_link_pads(audiosrc, "src", rtmpbin, "audiosink")) {
        g_printerr("RtmpGraph: ERROR: cannot link elements of types 'audioinputbin' and 'rtmpbin'\n");
        free();
        return;
    }
}

void RtmpGraph::free()
{
    if (m_pipeline) {
        gst_object_unref(GST_OBJECT(m_pipeline));
        m_pipeline = NULL;
    }
}
