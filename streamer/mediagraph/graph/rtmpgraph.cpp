#include "rtmpgraph.h"
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

    GstElement *videosrc = gst_element_factory_make("v4l2src", "videosrc");
    if (!videosrc) {
        g_printerr("RtmpGraph: ERROR: failed to create element of type 'v4l2src'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), videosrc)) {
        g_printerr("RtmpGraph: ERROR: bin doesn't want to accept element of type 'v4l2src'\n");
        gst_object_unref(GST_OBJECT(videosrc));
        free();
        return;
    }

    GstElement *audiosrc = gst_element_factory_make("alsasrc", "audiosrc");
    if (!audiosrc) {
        g_printerr("RtmpGraph: ERROR: failed to create element of type 'alsasrc'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), audiosrc)) {
        g_printerr("RtmpGraph: ERROR: bin doesn't want to accept element of type 'alsasrc'\n");
        gst_object_unref(GST_OBJECT(audiosrc));
        free();
        return;
    }

    RtmpStreamingBin bin(m_location);
    GstElement *rtmpbin = bin.get();
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
        g_printerr("RtmpGraph: ERROR: cannot link elements of types 'v4l2src' and 'rtmpbin'\n");
        free();
        return;
    }

    if (!gst_element_link_pads(audiosrc, "src", rtmpbin, "audiosink")) {
        g_printerr("RtmpGraph: ERROR: cannot link elements of types 'alsasrc' and 'rtmpbin'\n");
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
