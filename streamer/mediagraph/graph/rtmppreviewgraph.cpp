#include "rtmppreviewgraph.h"

#include <gst/gst.h>
#include <gst/video/videooverlay.h>

static gboolean tee_queue_link(GstElement *tee, GstElement *queue)
{
    GstPadTemplate *tee_src_pad_template = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(tee), "src_%u");
    GstPad *tee_pad = gst_element_request_pad(tee, tee_src_pad_template, NULL, NULL);
    GstPad *queue_pad = gst_element_get_static_pad(queue, "sink");

    gboolean link_ok = (gst_pad_link(tee_pad, queue_pad) == GST_PAD_LINK_OK);

    // TODO: release and unref requested pads
    gst_object_unref(queue_pad);

    return link_ok;
}

RtmpPreviewGraph::RtmpPreviewGraph()
    : AbstractGraph(),
      m_pipeline(NULL),
      m_winId(0)
{
    setup();
}

RtmpPreviewGraph::RtmpPreviewGraph(int winid, const std::string &location)
    : AbstractGraph(),
      m_pipeline(NULL),
      m_winId(winid),
      m_location(location)
{
    setup();
}

RtmpPreviewGraph::~RtmpPreviewGraph()
{
    free();
}

bool RtmpPreviewGraph::start()
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

void RtmpPreviewGraph::stop()
{
    if (!m_pipeline)
        error("ERROR: failed to stop streaming, pipeline is corrupted\n");

    gst_element_set_state(m_pipeline, GST_STATE_NULL);
}

void RtmpPreviewGraph::setup()
{
    // Create elements
    GstElement *webcam = gst_element_factory_make("v4l2src", "wcs-webcam");
    if (!webcam)
        error("ERROR: failed to create element of type 'v4l2src'\n");

    GstElement *tee = gst_element_factory_make("tee", "wcs-tee");
    if (!tee) {
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create element of type 'tee'\n");
    }

    GstElement *youtube_queue = gst_element_factory_make("queue", "wcs-yt-queue");
    if (!youtube_queue) {
        gst_object_unref(GST_OBJECT(tee));
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create element of type 'tee'\n");
    }

    GstElement *window_queue = gst_element_factory_make("queue", "wcs-wnd-queue");
    if (!window_queue) {
        gst_object_unref(GST_OBJECT(youtube_queue));
        gst_object_unref(GST_OBJECT(tee));
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create element for preview\n");
    }
    g_object_set(G_OBJECT(window_queue), "leaky", 1, NULL);

    GstElement *window = NULL;
    m_winId == 0
            ? window = gst_element_factory_make("autovideosink", "wcs-window")
            : window = gst_element_factory_make("xvimagesink", "wcs-window");
    if (!window) {
        gst_object_unref(GST_OBJECT(window_queue));
        gst_object_unref(GST_OBJECT(youtube_queue));
        gst_object_unref(GST_OBJECT(tee));
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create element of type 'videosink'"
              " or 'xvimagesink'\n");
    }

    GstElement *h264enc = gst_element_factory_make("x264enc", "wcs-h264encoder");
    if (!h264enc) {
        gst_object_unref(GST_OBJECT(window));
        gst_object_unref(GST_OBJECT(window_queue));
        gst_object_unref(GST_OBJECT(youtube_queue));
        gst_object_unref(GST_OBJECT(tee));
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create element of type 'x264enc'\n");
    }
    g_object_set(G_OBJECT(h264enc), "bitrate", 512, NULL);

    GstElement *microphone = gst_element_factory_make("alsasrc", "wcs-microphone");
    if (!microphone) {
        gst_object_unref(GST_OBJECT(h264enc));
        gst_object_unref(GST_OBJECT(window));
        gst_object_unref(GST_OBJECT(window_queue));
        gst_object_unref(GST_OBJECT(youtube_queue));
        gst_object_unref(GST_OBJECT(tee));
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create element of type 'alsasrc'\n");
    }
    g_object_set(G_OBJECT(microphone), "device", "hw:1", NULL);

    GstElement *faac = gst_element_factory_make("faac", "wcs-faac");
    if (!faac) {
        gst_object_unref(GST_OBJECT(microphone));
        gst_object_unref(GST_OBJECT(h264enc));
        gst_object_unref(GST_OBJECT(window));
        gst_object_unref(GST_OBJECT(window_queue));
        gst_object_unref(GST_OBJECT(youtube_queue));
        gst_object_unref(GST_OBJECT(tee));
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create element of type 'faac'\n");
    }
    g_object_set(G_OBJECT(faac), "bitrate", 32000, NULL);

    GstElement *flvmux = gst_element_factory_make("flvmux", "wcs-flvmux");
    if (!flvmux) {
        gst_object_unref(GST_OBJECT(faac));
        gst_object_unref(GST_OBJECT(microphone));
        gst_object_unref(GST_OBJECT(h264enc));
        gst_object_unref(GST_OBJECT(window));
        gst_object_unref(GST_OBJECT(window_queue));
        gst_object_unref(GST_OBJECT(youtube_queue));
        gst_object_unref(GST_OBJECT(tee));
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create element of type 'flvmux'\n");
    }

    GstElement *rtmp = gst_element_factory_make("rtmpsink", "wcs-rtmp");
    if (!rtmp) {
        gst_object_unref(GST_OBJECT(flvmux));
        gst_object_unref(GST_OBJECT(faac));
        gst_object_unref(GST_OBJECT(microphone));
        gst_object_unref(GST_OBJECT(h264enc));
        gst_object_unref(GST_OBJECT(window));
        gst_object_unref(GST_OBJECT(window_queue));
        gst_object_unref(GST_OBJECT(youtube_queue));
        gst_object_unref(GST_OBJECT(tee));
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create element of type 'rtmpsink'\n");
    }
    g_object_set(G_OBJECT(rtmp), "location", m_location.c_str(), NULL);

    m_pipeline = gst_pipeline_new("wcs-pipeline");
    if (!m_pipeline) {
        gst_object_unref(GST_OBJECT(rtmp));
        gst_object_unref(GST_OBJECT(flvmux));
        gst_object_unref(GST_OBJECT(faac));
        gst_object_unref(GST_OBJECT(microphone));
        gst_object_unref(GST_OBJECT(h264enc));
        gst_object_unref(GST_OBJECT(window));
        gst_object_unref(GST_OBJECT(window_queue));
        gst_object_unref(GST_OBJECT(youtube_queue));
        gst_object_unref(GST_OBJECT(tee));
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create pipeline\n");
    }

    // Add elements to pipeline
    gst_bin_add_many(GST_BIN(m_pipeline), webcam, tee, youtube_queue, window_queue, window, h264enc, microphone, faac, flvmux, rtmp, NULL);

    // Link elements
    if (!gst_element_link(webcam, tee))
        error("ERROR: failed to link elements of types 'v4l2src' and 'tee'\n");

    if (!gst_element_link(window_queue, window))
        error("ERROR: failed to link elements of types 'preview_queue' and 'autovideosink'\n");

    if (!gst_element_link(youtube_queue, h264enc))
        error("ERROR: failed to link elements of types 'youtube_queue' and 'autovideosink'\n");

    if (!gst_element_link_pads(h264enc, "src", flvmux, "video"))
        error("ERROR: failed to link elements of types 'x264enc' and 'flvmux'\n");

    if (!gst_element_link(microphone , faac))
        error("ERROR: failed to link elements of types 'alsasrc' and 'faac'\n");

    if (!gst_element_link_pads_filtered(faac, "src", flvmux, "audio", NULL))
        error("ERROR: failed to link elements of types 'faac' and 'flvmux'\n");

    if (!gst_element_link(flvmux, rtmp))
        error("ERROR: failed to link elements of types 'flvmux' and 'rtmpsink'\n");

    if (!tee_queue_link(tee, window_queue))
        error("ERROR: failed to link elements of types 'tee' and 'queue' (window)\n");

    if (!tee_queue_link(tee, youtube_queue))
        error("ERROR: failed to link elements of types 'tee' and 'queue' (youtube)\n");
}

void RtmpPreviewGraph::free()
{
    if (m_pipeline)
        gst_object_unref(GST_OBJECT(m_pipeline));
}

void RtmpPreviewGraph::cleanup()
{
    free();
}
