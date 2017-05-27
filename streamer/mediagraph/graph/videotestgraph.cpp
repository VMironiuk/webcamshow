#include "videotestgraph.h"

#include <gst/gst.h>

VideoTestGraph::VideoTestGraph()
    : AbstractGraph(),
      m_pipeline(NULL)
{
    setup();
}

VideoTestGraph::~VideoTestGraph()
{
    free();
}

bool VideoTestGraph::start()
{
    if (!m_pipeline) {
        g_printerr("ERROR: failed to start streaming, pipeline is corrupted\n");
        return FALSE;
    }

    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

    return TRUE;
}

void VideoTestGraph::stop()
{
    if (!m_pipeline) {
        g_printerr("ERROR: failed to stop streaming, pipeline is corrupted\n");
        return;
    }

    gst_element_set_state(m_pipeline, GST_STATE_NULL);
}

void VideoTestGraph::setup()
{
    // Create elements
    m_pipeline = gst_pipeline_new("pipeline");
    if (!m_pipeline) {
        g_printerr("VideoTesGraph: ERROR: failed to create element of type 'pipeline'\n");
        return;
    }

    GstElement *videosrc = gst_element_factory_make("videotestsrc", "videosrc");
    if (!videosrc) {
        g_printerr("VideoTesGraph: ERROR: failed to create element of type 'videotestsrc'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), videosrc)) {
        g_printerr("VideoTesGraph: ERROR: pipeline doesn't want to accept element of type 'videotestsrc'\n");
        gst_object_unref(GST_OBJECT(videosrc));
        free();
        return;
    }

    GstElement *videosink = gst_element_factory_make("autovideosink", "videosink");
    if (!videosink) {
        g_printerr("VideoTesGraph: ERROR: failed to create element of type 'autovideosink'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), videosink)) {
        g_printerr("VideoTesGraph: ERROR: pipeline doesn't want to accept element of type 'autovidesionk'\n");
        gst_object_unref(GST_OBJECT(videosink));
        free();
        return;
    }

    // Link elelemnts
    if (!gst_element_link(videosrc, videosink)) {
        g_printerr("VideoTesGraph: ERROR: failed to link elements of types "
                   "'videotestsrc' and 'autovideosink'\n");
        free();
        return;
    }
}

void VideoTestGraph::free()
{
    if (m_pipeline) {
        gst_object_unref(GST_OBJECT(m_pipeline));
        m_pipeline = NULL;
    }
}

void VideoTestGraph::cleanup()
{
    free();
}
