#include "rtmppreviewvideotestgraph.h"
#include "videotestbin.h"
#include "audiotestbin.h"
#include "rtmpstreamingbin.h"
#include "videopreviewbin.h"

#include <gst/gst.h>
#include <gst/video/videooverlay.h>


RtmpPreviewVideoTestGraph::RtmpPreviewVideoTestGraph()
    :AbstractGraph(),
      m_pipeline(NULL),
      m_winId(0)
{
    setup();
}

RtmpPreviewVideoTestGraph::RtmpPreviewVideoTestGraph(int winid, const std::string &location)
    : AbstractGraph(),
      m_pipeline(NULL),
      m_winId(winid),
      m_location(location)
{
    setup();
}

RtmpPreviewVideoTestGraph::~RtmpPreviewVideoTestGraph()
{
    free();
}

bool RtmpPreviewVideoTestGraph::start()
{
    if (!m_pipeline) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: failed to start streaming, pipeline is corrupted\n");
        return FALSE;
    }

    if (m_winId != 0) {
        GstElement *sink = gst_bin_get_by_name(GST_BIN(m_pipeline), "videosink");
        gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(sink), m_winId);
    }

    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

    return TRUE;
}

void RtmpPreviewVideoTestGraph::stop()
{
    if (!m_pipeline) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: failed to stop streaming, pipeline is corrupted\n");
        return;
    }

    gst_element_set_state(m_pipeline, GST_STATE_NULL);
}

void RtmpPreviewVideoTestGraph::setup()
{
    // Create elements
    m_pipeline = gst_pipeline_new("pipeline");
    if (!m_pipeline) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: failed to create element of type 'pipeline'\n");
        return;
    }

    VideoTestBin videotestbin;
    GstElement *videosrc = videotestbin.get();
    if (!videosrc) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: failed to create element of type 'videotestbin'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), videosrc)) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: pipeline doesn't want to accept element of type 'videotestbin'\n");
        gst_object_unref(GST_OBJECT(videosrc));
        free();
        return;
    }

    GstElement *tee = gst_element_factory_make("tee", "tee");
    if (!tee) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: failed to create element of type 'tee'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), tee)) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: pipeline doesn't want to accept element of type 'tee'\n");
        gst_object_unref(GST_OBJECT(tee));
        free();
        return;
    }

    AudioTestBin audiotestbin;
    GstElement *audiosrc = audiotestbin.get();
    if (!audiosrc) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: failed to create element of type 'audiotestbin'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), audiosrc)) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: pipeline doesn't want to accept element of type 'audiotestbin'\n");
        gst_object_unref(GST_OBJECT(audiosrc));
        free();
        return;
    }

    RtmpStreamingBin rtmpstreamingbin(m_location);
    GstElement *rtmpbin = rtmpstreamingbin.get();
    if (!rtmpbin) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: failed to create element of type 'rtmpbin'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), rtmpbin)) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: pipeline doesn't want to accept element of type 'rtmpbin'\n");
        gst_object_unref(GST_OBJECT(rtmpbin));
        free();
        return;
    }

    VideoPreviewBin videopreviewbin(m_winId);
    GstElement *previewbin = videopreviewbin.get();
    if (!previewbin) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: failed to create element of type 'videopreviewbin'\n");
        free();
        return;
    }

    if (!gst_bin_add(GST_BIN(m_pipeline), previewbin)) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: pipeline doesn't want to accept element of type 'videopreviewbin'\n");
        gst_object_unref(GST_OBJECT(previewbin));
        free();
        return;
    }

    // Link elements
    GstPadTemplate *tee_src_pad_template = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(tee), "src_%u");
    if (!tee_src_pad_template) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: none pad template of type 'tee' was found\n");
        free();
        return;
    }

    // Link tee ! preview
    GstPad *tee_pad_src_0 = gst_element_request_pad(tee, tee_src_pad_template, NULL, NULL);
    g_assert(tee_pad_src_0);
    m_requested_pads.push_back(tee_pad_src_0);

    GstPad *preview_pad = gst_element_get_static_pad(previewbin, VideoPreviewBin::GHOST_PAD_NAME);
    g_assert(preview_pad);

    if (gst_pad_link(tee_pad_src_0, preview_pad) != GST_PAD_LINK_OK) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: cannot link 'tee' and 'videopreviewbin'\n");
        gst_object_unref(GST_OBJECT(preview_pad));
        free();
        return;
    }

    // Link tee ! rtmp
    GstPad *tee_pad_src_1 = gst_element_request_pad(tee, tee_src_pad_template, NULL, NULL);
    g_assert(tee_pad_src_1);
    m_requested_pads.push_back(tee_pad_src_1);

    GstPad *rtmp_pad = gst_element_get_static_pad(rtmpbin, RtmpStreamingBin::VIDEO_GHOST_PAD_NAME);
    g_assert(rtmp_pad);

    if (gst_pad_link(tee_pad_src_1, rtmp_pad) != GST_PAD_LINK_OK) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: cannot link 'tee' and 'rtmpstreamingbin'\n");
        gst_object_unref(GST_OBJECT(preview_pad));
        gst_object_unref(GST_OBJECT(rtmp_pad));
        free();
        return;
    }

    gst_object_unref(GST_OBJECT(preview_pad));
    gst_object_unref(GST_OBJECT(rtmp_pad));

    // Link videosrc ! tee
    if (!gst_element_link(videosrc, tee)) {
        g_printerr("RtmpPreviewGraph: ERROR: failed to link elements of types 'videotestbin' and 'tee'\n");
        free();
        return;
    }

    // Link audiosrc ! rtmpbin
    if (!gst_element_link_pads(audiosrc, AudioTestBin::GHOST_PAD_NAME, rtmpbin, RtmpStreamingBin::AUDIO_GHOST_PAD_NAME)) {
        g_printerr("RtmpPreviewVideoTestGraph: ERROR: failed to link elements of types 'audiotestbin' and 'rtmpbin'\n");
        free();
        return;
    }
}

void RtmpPreviewVideoTestGraph::free()
{
    if (m_pipeline) {
        // Try to release and unref requested pads
        GstElement *tee = gst_bin_get_by_name(GST_BIN(m_pipeline), "tee");
        if (tee) {
            for (auto pad : m_requested_pads) {
                if (pad) {
                    gst_element_release_request_pad(tee, pad);
                    gst_object_unref(GST_OBJECT(pad));
                }
            }
            gst_object_unref(GST_OBJECT(tee));
        }

        gst_object_unref(GST_OBJECT(m_pipeline));
        m_pipeline = NULL;
    }
}
