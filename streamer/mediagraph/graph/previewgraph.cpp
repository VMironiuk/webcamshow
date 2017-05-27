#include "previewgraph.h"

#include <gst/gst.h>
#include <gst/video/videooverlay.h>

PreviewGraph::PreviewGraph()
    : AbstractGraph(),
      m_pipeline(NULL),
      m_winId(0)
{
    setup();
}

PreviewGraph::PreviewGraph(int winId)
    : AbstractGraph(),
      m_pipeline(NULL),
      m_winId(winId)
{
    setup();
}

PreviewGraph::~PreviewGraph()
{
    free();
}

bool PreviewGraph::start()
{
    if (!m_pipeline) {
        g_printerr("ERROR: failed to start streaming, pipeline is corrupted\n");
        return FALSE;
    }

    if (m_winId != 0) {
        GstElement *sink = gst_bin_get_by_name(GST_BIN(m_pipeline), "wcs-window");
        gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(sink), m_winId);
    }

    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

    return TRUE;
}

void PreviewGraph::stop()
{
    if (!m_pipeline)
        error("ERROR: failed to stop streaming, pipeline is corrupted\n");

    gst_element_set_state(m_pipeline, GST_STATE_NULL);
}

void PreviewGraph::setup()
{
    // Create elements
    GstElement *webcam = gst_element_factory_make("v4l2src", "wcs-webcam");
    if (!webcam)
        error("ERROR: failed to create element of type 'v4l2src'\n");

    GstElement *window = NULL;
    m_winId == 0
            ? window = gst_element_factory_make("autovideosink", "wcs-window")
            : window = gst_element_factory_make("xvimagesink", "wcs-window");
    if (!window) {
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create element of type 'videosink'"
                             " or 'xvimagesink'\n");
    }

    m_pipeline = gst_pipeline_new("wcs-pipeline");
    if (!m_pipeline) {
        gst_object_unref(GST_OBJECT(window));
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create pipeline\n");
    }

    gst_bin_add_many(GST_BIN(m_pipeline), webcam, window, NULL);
    if (!gst_element_link(webcam, window)) {
        error("ERROR: failed to link elements of types "
              "'v4l2src' and 'autovidesink' or 'xvimagesink'\n");
    }
}

void PreviewGraph::free()
{
    if (m_pipeline)
        gst_object_unref(GST_OBJECT(m_pipeline));
}

void PreviewGraph::cleanup()
{
    free();
}
