#include "videotestgraph.h"
#include "videotestbin.h"
#include "videopreviewbin.h"

#include <gst/gst.h>
#include <gst/video/videooverlay.h>

VideoTestGraph::VideoTestGraph()
    : AbstractGraph(),
      m_pipeline(NULL),
      m_winId(0)
{
    setup();
}

VideoTestGraph::VideoTestGraph(int winId)
    : AbstractGraph(),
      m_pipeline(NULL),
      m_winId(winId)
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
        g_printerr("VideoTestGraph: ERROR: failed to start streaming, pipeline is corrupted\n");
        return FALSE;
    }

    if (m_winId != 0) {
        GstElement *sink = gst_bin_get_by_name(GST_BIN(m_pipeline), "videosink");
        gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(sink), m_winId);
    }

    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

    return TRUE;
}

void VideoTestGraph::stop()
{
    if (!m_pipeline) {
        g_printerr("VideoTestGraph: ERROR: failed to stop streaming, pipeline is corrupted\n");
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

    VideoTestBin videotestbin;
    GstElement *videosrc = videotestbin.get();
    if (!videosrc) {
        g_printerr("VideoTesGraph: ERROR: failed to create element of type 'videotestbin'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), videosrc)) {
        g_printerr("VideoTesGraph: ERROR: pipeline doesn't want to accept element of type 'videotestbin'\n");
        gst_object_unref(GST_OBJECT(videosrc));
        free();
        return;
    }

    // TODO: video test streaming is hangs while linked to VideoPreviewBin
    GstElement *videosink = NULL;
    m_winId == 0
            ? videosink = gst_element_factory_make("autovideosink", "videosink")
            : videosink = gst_element_factory_make("xvimagesink", "videosink");
    if (!videosink) {
        g_printerr("VideoTesGraph: ERROR: failed to create element of type 'videosink'\n");
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
                   "'videotestbin' and 'videosink'\n");
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
