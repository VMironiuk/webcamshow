#include "rtmpgraph.h"

#include <gst/gst.h>

static gboolean v4l2src_x264enc_link_filtered(GstElement *v4l2src, GstElement *x264enc)
{
    gboolean link_ok;
    GstCaps *caps;

    caps = gst_caps_new_simple("video/x-raw",
                               "width", G_TYPE_INT, 640,
                               "height", G_TYPE_INT, 480,
                               "framerate", GST_TYPE_FRACTION, 15, 1,
                               NULL);
    if (!caps) {
        g_printerr("ERROR: failed to create caps while linking"
                " elements of types 'v4l2src' and 'x264enc'\n");
        return FALSE;
    }

    link_ok = gst_element_link_filtered(v4l2src, x264enc, caps);
    gst_caps_unref(caps);

    return link_ok;
}

static gboolean x264enc_flvmux_link_filtered(GstElement *x264enc, GstElement *flvmux)
{
    gboolean link_ok;
    GstCaps *caps;

    caps = gst_caps_new_simple("video/x-h264",
                               "profile", G_TYPE_STRING, "high",
                               NULL);
    if (!caps) {
        g_printerr("ERROR: failed to create caps while linking"
                " elements of types 'x264enc' and 'flvmux'\n");
        return FALSE;
    }

    link_ok = gst_element_link_pads_filtered(x264enc, "src", flvmux, "video", caps);
    gst_caps_unref(caps);

    return link_ok;
}

static gboolean alsasrc_faac_link_filtered(GstElement *alsasrc, GstElement *faac)
{
    gboolean link_ok;
    GstCaps *caps;

    caps = gst_caps_new_simple("audio/x-raw",
                               "rate", G_TYPE_INT, 44100,
                               NULL);
    if (!caps) {
        g_printerr("ERROR: failed to create caps while linking"
                " elements of types 'alsasrc' and 'faac'\n");
        return FALSE;
    }

    link_ok = gst_element_link_filtered(alsasrc, faac, caps);
    gst_caps_unref(caps);

    return link_ok;
}

RtmpGraph::RtmpGraph()
    : AbstractGraph(),
      m_pipeline(NULL)
{
    setup();
}

RtmpGraph::RtmpGraph(const std::string &rtmp_location)
    : AbstractGraph(),
      m_pipeline(NULL),
      m_rtmp_location(rtmp_location)
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
        g_printerr("ERROR: failed to start streaming, pipeline is corrupted\n");
        return FALSE;
    }

    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

    return TRUE;
}

void RtmpGraph::stop()
{
    if (!m_pipeline)
        error("ERROR: failed to stop streaming, pipeline is corrupted\n");

    gst_element_set_state(m_pipeline, GST_STATE_NULL);
}

void RtmpGraph::setup()
{
    // Create elements
    GstElement *webcam = gst_element_factory_make("v4l2src", "wcs-webcam");
    if (!webcam)
        error("ERROR: failed to create element of type 'v4l2src'\n");

    GstElement *h264enc = gst_element_factory_make("x264enc", "wcs-h264encoder");
    if (!h264enc) {
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create element of type 'x264enc'\n");
    }
    g_object_set(G_OBJECT(h264enc), "bitrate", 1000, NULL);

    GstElement *microphone = gst_element_factory_make("alsasrc", "wcs-microphone");
    if (!microphone) {
        gst_object_unref(GST_OBJECT(h264enc));
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create element of type 'alsasrc'\n");
    }
    g_object_set(G_OBJECT(microphone), "device", "hw:1", NULL);

    GstElement *faac = gst_element_factory_make("faac", "wcs-faac");
    if (!faac) {
        gst_object_unref(GST_OBJECT(microphone));
        gst_object_unref(GST_OBJECT(h264enc));
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create element of type 'faac'\n");
    }
    g_object_set(G_OBJECT(faac), "bitrate", 128000, NULL);

    GstElement *flvmux = gst_element_factory_make("flvmux", "wcs-flvmux");
    if (!flvmux) {
        gst_object_unref(GST_OBJECT(faac));
        gst_object_unref(GST_OBJECT(microphone));
        gst_object_unref(GST_OBJECT(h264enc));
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create element of type 'flvmux'\n");
    }

    GstElement *rtmp = gst_element_factory_make("rtmpsink", "wcs-rtmp");
    if (!rtmp) {
        gst_object_unref(GST_OBJECT(flvmux));
        gst_object_unref(GST_OBJECT(faac));
        gst_object_unref(GST_OBJECT(microphone));
        gst_object_unref(GST_OBJECT(h264enc));
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create element of type 'rtmpsink'\n");
    }
    g_object_set(G_OBJECT(rtmp), "location", m_rtmp_location.c_str(), NULL);

    m_pipeline = gst_pipeline_new("wcs-pipeline");
    if (!m_pipeline) {
        gst_object_unref(GST_OBJECT(rtmp));
        gst_object_unref(GST_OBJECT(flvmux));
        gst_object_unref(GST_OBJECT(faac));
        gst_object_unref(GST_OBJECT(microphone));
        gst_object_unref(GST_OBJECT(h264enc));
        gst_object_unref(GST_OBJECT(webcam));
        error("ERROR: failed to create pipeline\n");
    }

    // Add elements to pipeline
    gst_bin_add_many(GST_BIN(m_pipeline), webcam, h264enc, microphone, faac, flvmux, rtmp, NULL);

    // Link elements
    if (!v4l2src_x264enc_link_filtered(webcam, h264enc))
        error("ERROR: failed to link elements of types 'v4l2src' and 'x264enc'\n");

    if (!x264enc_flvmux_link_filtered(h264enc, flvmux))
        error("ERROR: failed to link elements of types 'x264enc' and 'flvmux'\n");

    if (!alsasrc_faac_link_filtered(microphone, faac))
        error("ERROR: failed to link elements of types 'alsasrc' and 'faac'\n");

    if (!gst_element_link_pads_filtered(faac, "src", flvmux, "audio", NULL))
        error("ERROR: failed to link elements of types 'faac' and 'flvmux'\n");

    if (!gst_element_link(flvmux, rtmp))
        error("ERROR: failed to link element of types 'flvmux' and 'rtmpsink'");
}

void RtmpGraph::free()
{
    if (m_pipeline)
        gst_object_unref(GST_OBJECT(m_pipeline));
}

void RtmpGraph::cleanup()
{
    free();
}
