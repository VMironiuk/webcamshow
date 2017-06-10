#include "previewgraph.h"
#include "videoinputbin.h"
#include "videopreviewbin.h"

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
        g_printerr("PreviewGraph: ERROR: failed to start streaming, pipeline is corrupted\n");
        return FALSE;
    }

    if (m_winId != 0) {
        GstElement *sink = gst_bin_get_by_name(GST_BIN(m_pipeline), "videosink");
        gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(sink), m_winId);
    }

    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

    return TRUE;
}

void PreviewGraph::stop()
{
    if (!m_pipeline) {
        g_printerr("PreviewGraph: ERROR: failed to stop streaming, pipeline is corrupted\n");
        return;
    }

    gst_element_set_state(m_pipeline, GST_STATE_NULL);
}

void PreviewGraph::setup()
{
    // Create elements
    m_pipeline = gst_pipeline_new("pipeline");
    if (!m_pipeline) {
        g_printerr("PreviewGraph: ERROR: failed to create element of type 'pipeline'\n");
        return;
    }

    VideoInputBin inputbin;
    GstElement *videosrc = inputbin.get();
    if (!videosrc) {
        g_printerr("PreviewGraph: ERROR: failed to create element of type 'videoinputbin'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), videosrc)) {
        g_printerr("PreviewGraph: ERROR: pipeline doesn't want to accept element of type 'videoinputbin'\n");
        gst_object_unref(GST_OBJECT(videosrc));
        free();
        return;
    }


    VideoPreviewBin previewbin(m_winId);
    GstElement *videopreviewbin = previewbin.get();
    if (!videopreviewbin) {
        g_printerr("PreviewGraph: ERROR: failed to create element of type 'videopreviewbin'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), videopreviewbin)) {
        g_printerr("PreviewGraph: ERROR: pipeline doesn't want to accept element of type 'videopreviewbin'\n");
        gst_object_unref(GST_OBJECT(videopreviewbin));
        free();
        return;
    }

    // Link elements
    if (!gst_element_link(videosrc, videopreviewbin)) {
        g_printerr("PreviewGraph: ERROR: failed to link elements of types "
                   "'videoinputbin' and 'videopreviewbin'\n");
        free();
        return;
    }
}

void PreviewGraph::free()
{
    if (m_pipeline) {
        gst_object_unref(GST_OBJECT(m_pipeline));
        m_pipeline = NULL;
    }
}
