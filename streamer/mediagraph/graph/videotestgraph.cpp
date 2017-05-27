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
    if (!m_pipeline)
        error("ERROR: failed to stop streaming, pipeline is corrupted\n");

    gst_element_set_state(m_pipeline, GST_STATE_NULL);
}

void VideoTestGraph::setup()
{
    // Create elements
    GstElement *video_test = gst_element_factory_make("videotestsrc", "wcs-videotest");
    if (!video_test)
        error("ERROR: failed to create element of type 'videosrc'\n");

    GstElement *video_sink = gst_element_factory_make("autovideosink", "wcs-videosink");
    if (!video_sink) {
        gst_object_unref(GST_OBJECT(video_test));
        error("ERROR: failed to create element of type 'videosrc'\n");
    }

    m_pipeline = gst_pipeline_new("wcs-pipeline");
    if (!m_pipeline) {
        gst_object_unref(GST_OBJECT(video_sink));
        gst_object_unref(GST_OBJECT(video_test));
        error("ERROR: failed to create pipeline\n");
    }

    gst_bin_add_many(GST_BIN(m_pipeline), video_test, video_sink, NULL);
    if (!gst_element_link(video_test, video_sink)) {
        error("ERROR: failed to link elements of types "
              "'videotestsrc' and 'autovidesink'\n");
    }
}

void VideoTestGraph::free()
{
    if (m_pipeline)
        gst_object_unref(GST_OBJECT(m_pipeline));
}

void VideoTestGraph::cleanup()
{
    free();
}
