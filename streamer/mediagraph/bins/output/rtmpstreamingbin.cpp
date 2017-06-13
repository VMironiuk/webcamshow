#include "rtmpstreamingbin.h"

#include <gst/gst.h>

const char *RtmpStreamingBin::BIN_NAME = "rtmpstreamingbin";
const char *RtmpStreamingBin::MUXER_NAME = "flvmux";
const char *RtmpStreamingBin::MUXER_ALIAS = "muxer";
const char *RtmpStreamingBin::SINK_NAME = "rtmpsink";
const char *RtmpStreamingBin::SINK_ALIAS = "rtmpsink";
const char *RtmpStreamingBin::QUEUE_NAME = "queue";
const char *RtmpStreamingBin::AUDIO_QUEUE_ALIAS = "audioqueue";
const char *RtmpStreamingBin::VIDEO_QUEUE_ALIAS = "videoqueue";
const char *RtmpStreamingBin::FAAC_NAME = "faac";
const char *RtmpStreamingBin::FAAC_ALIAS = "audiocodec";
const char *RtmpStreamingBin::AUDIO_GHOST_PAD_NAME = "audiosink";
const char *RtmpStreamingBin::H264_NAME = "x264enc";
const char *RtmpStreamingBin::H264_ALIAS = "videocodec";
const char *RtmpStreamingBin::VIDEO_GHOST_PAD_NAME = "videosink";

RtmpStreamingBin::RtmpStreamingBin(const std::string &location)
    : AbstractBin(),
      m_location(location)
{

}

RtmpStreamingBin::~RtmpStreamingBin()
{

}

GstElement *RtmpStreamingBin::get()
{
    if (m_location.empty()) {
        g_printerr("RtmpStreamingBin: ERROR: RTMP location is empty\n");
        return NULL;
    }

    // Create elements
    GstElement *bin = gst_bin_new(BIN_NAME);
    if (!bin) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type 'bin'\n");
        return NULL;
    }

    GstElement *muxer = gst_element_factory_make(MUXER_NAME, MUXER_ALIAS);
    if (!muxer) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type '%s'\n", MUXER_NAME);
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_bin_add(GST_BIN(bin), muxer)) {
        g_printerr("RtmpStreamingBin: ERROR: bin doesn't want to accept element of type '%s'\n", MUXER_NAME);
        gst_object_unref(GST_OBJECT(bin));
        gst_object_unref(GST_OBJECT(muxer));
        return NULL;
    }

    GstElement *rtmpsink = gst_element_factory_make(SINK_NAME, SINK_ALIAS);
    if (!rtmpsink) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type '%s'\n", SINK_NAME);
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }
    g_object_set(G_OBJECT(rtmpsink), "location", m_location.c_str(), NULL);

    if (!gst_bin_add(GST_BIN(bin), rtmpsink)) {
        g_printerr("RtmpStreamingBin: ERROR: bin doesn't want to accept element of type '%s'\n", SINK_NAME);
        gst_object_unref(GST_OBJECT(bin));
        gst_object_unref(GST_OBJECT(rtmpsink));
        return NULL;
    }

    // Setup audio pipeline
    GstElement *audioqueue = gst_element_factory_make(QUEUE_NAME, AUDIO_QUEUE_ALIAS);
    if (!audioqueue) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type '%s'\n", AUDIO_QUEUE_ALIAS);
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_bin_add(GST_BIN(bin), audioqueue)) {
        g_printerr("RtmpStreamingBin: ERROR: bin doesn't want to accept element of type '%s'\n", AUDIO_QUEUE_ALIAS);
        gst_object_unref(GST_OBJECT(bin));
        gst_object_unref(GST_OBJECT(audioqueue));
        return NULL;
    }

    GstElement *audiocodec = gst_element_factory_make(FAAC_NAME, FAAC_ALIAS);
    if (!audiocodec) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type '%s'\n", FAAC_NAME);
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_bin_add(GST_BIN(bin), audiocodec)) {
        g_printerr("RtmpStreamingBin: ERROR: bin doesn't want to accept element of type '%s'\n", FAAC_NAME);
        gst_object_unref(GST_OBJECT(bin));
        gst_object_unref(GST_OBJECT(audiocodec));
        return NULL;
    }

    // Set up ghost pad
    GstPad *pad = gst_element_get_static_pad(audioqueue, "sink");
    g_assert(pad);

    GstPad *audio_ghostpad = gst_ghost_pad_new(AUDIO_GHOST_PAD_NAME, pad);
    if (!audio_ghostpad) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type 'audioghostpad'\n");
        gst_object_unref(GST_OBJECT(pad));
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_element_add_pad(bin, audio_ghostpad)) {
        g_printerr("RtmpStreamingBin: ERROR: failed while adding audio ghost pad. Pad with the "
                   "same name already existed or the pad already had another parent\n");
        gst_object_unref(GST_OBJECT(audio_ghostpad));
        gst_object_unref(GST_OBJECT(pad));
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    gst_object_unref(GST_OBJECT(pad));

    // Link elements
    if (!gst_element_link(audioqueue, audiocodec)) {
        g_printerr("VideoPreview: ERROR: failed to link 'audioqueue' and 'audiocodec'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_element_link_pads(audiocodec, "src", muxer, "audio")) {
        g_printerr("VideoPreview: ERROR: failed to link 'audiocodec' and 'flvmux'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    // Setup video pipeline
    GstElement *videoqueue = gst_element_factory_make(QUEUE_NAME, VIDEO_QUEUE_ALIAS);
    if (!videoqueue) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type '%s'\n", VIDEO_QUEUE_ALIAS);
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_bin_add(GST_BIN(bin), videoqueue)) {
        g_printerr("RtmpStreamingBin: ERROR: bin doesn't want to accept element of type '%s'\n", VIDEO_QUEUE_ALIAS);
        gst_object_unref(GST_OBJECT(bin));
        gst_object_unref(GST_OBJECT(videoqueue));
        return NULL;
    }

    GstElement *videocodec = gst_element_factory_make(H264_NAME, H264_ALIAS);
    if (!videocodec) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type '%s'\n", H264_NAME);
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }
    g_object_set(G_OBJECT(videocodec), "tune", 4, "pass", 17, NULL);

    if (!gst_bin_add(GST_BIN(bin), videocodec)) {
        g_printerr("RtmpStreamingBin: ERROR: bin doesn't want to accept element of type '%s'\n", H264_NAME);
        gst_object_unref(GST_OBJECT(bin));
        gst_object_unref(GST_OBJECT(videocodec));
        return NULL;
    }

    // Setup ghost pad
    pad = gst_element_get_static_pad(videoqueue, "sink");
    g_assert(pad);

    GstPad *video_ghostpad = gst_ghost_pad_new(VIDEO_GHOST_PAD_NAME, pad);
    if (!video_ghostpad) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type 'videoghostpad'\n");
        gst_object_unref(GST_OBJECT(pad));
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_element_add_pad(bin, video_ghostpad)) {
        g_printerr("RtmpStreamingBin: ERROR: failed while adding video ghost pad. Pad with the "
                   "same name already existed or the pad already had another parent\n");
        gst_object_unref(GST_OBJECT(video_ghostpad));
        gst_object_unref(GST_OBJECT(pad));
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    gst_object_unref(GST_OBJECT(pad));

    // Link elements
    if (!gst_element_link(videoqueue, videocodec)) {
        g_printerr("VideoPreview: ERROR: failed to link 'videoqueue' and 'x264enc'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_element_link_pads(videocodec, "src", muxer, "video")) {
        g_printerr("VideoPreview: ERROR: failed to link 'x264enc' and 'flvmux'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    // Link flvmux to rtmpsink
    if (!gst_element_link(muxer, rtmpsink)) {
        g_printerr("VideoPreview: ERROR: failed to link 'flvmux' and 'rtmpsink'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    return bin;
}
