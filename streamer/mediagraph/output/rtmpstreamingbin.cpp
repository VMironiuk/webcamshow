#include "rtmpstreamingbin.h"

#include <gst/gst.h>

RtmpStreamingBin::RtmpStreamingBin(const std::string &location)
    : AbstractOutputBin(),
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
    GstElement *bin = gst_bin_new("rtmpstreamingbin");
    if (!bin) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type 'bin'\n");
        return NULL;
    }

    GstElement *muxer = gst_element_factory_make("flvmux", "muxer");
    if (!muxer) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type 'flvmux'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_bin_add(GST_BIN(bin), muxer)) {
        g_printerr("RtmpStreamingBin: ERROR: bin doesn't want to accept element of type 'flvmux'\n");
        gst_object_unref(GST_OBJECT(bin));
        gst_object_unref(GST_OBJECT(muxer));
        return NULL;
    }

    GstElement *rtmpsink = gst_element_factory_make("rtmpsink", "rtmpsink");
    if (!rtmpsink) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type 'rtmpsink'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }
    g_object_set(G_OBJECT(rtmpsink), "location", m_location.c_str(), NULL);

    if (!gst_bin_add(GST_BIN(bin), rtmpsink)) {
        g_printerr("RtmpStreamingBin: ERROR: bin doesn't want to accept element of type 'rtmpsink'\n");
        gst_object_unref(GST_OBJECT(bin));
        gst_object_unref(GST_OBJECT(rtmpsink));
        return NULL;
    }

    // Setup audio pipeline
    GstElement *audioqueue = gst_element_factory_make("queue", "audioqueue");
    if (!audioqueue) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type 'audioqueue'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_bin_add(GST_BIN(bin), audioqueue)) {
        g_printerr("RtmpStreamingBin: ERROR: bin doesn't want to accept element of type 'audioqueue'\n");
        gst_object_unref(GST_OBJECT(bin));
        gst_object_unref(GST_OBJECT(audioqueue));
        return NULL;
    }

    GstElement *audiocodec = gst_element_factory_make("faac", "audiocodec");
    if (!audiocodec) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type 'faac'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_bin_add(GST_BIN(bin), audiocodec)) {
        g_printerr("RtmpStreamingBin: ERROR: bin doesn't want to accept element of type 'faac'\n");
        gst_object_unref(GST_OBJECT(bin));
        gst_object_unref(GST_OBJECT(audiocodec));
        return NULL;
    }

    // Set up ghost pad
    GstPad *pad = gst_element_get_static_pad(audioqueue, "sink");
    g_assert(pad);

    GstPad *audio_ghostpad = gst_ghost_pad_new("audiosink", pad);
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
    GstElement *videoqueue = gst_element_factory_make("queue", "videoqueue");
    if (!videoqueue) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type 'videoqueue'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_bin_add(GST_BIN(bin), videoqueue)) {
        g_printerr("RtmpStreamingBin: ERROR: bin doesn't want to accept element of type 'videoqueue'\n");
        gst_object_unref(GST_OBJECT(bin));
        gst_object_unref(GST_OBJECT(videoqueue));
        return NULL;
    }

    GstElement *videocodec = gst_element_factory_make("x264enc", "videocodec");
    if (!videocodec) {
        g_printerr("RtmpStreamingBin: ERROR: failed to create element of type 'x264enc'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }
    g_object_set(G_OBJECT(videocodec), "tune", 4, "pass", 17, NULL);

    if (!gst_bin_add(GST_BIN(bin), videocodec)) {
        g_printerr("RtmpStreamingBin: ERROR: bin doesn't want to accept element of type 'x264enc'\n");
        gst_object_unref(GST_OBJECT(bin));
        gst_object_unref(GST_OBJECT(videocodec));
        return NULL;
    }

    // Setup ghost pad
    pad = gst_element_get_static_pad(videoqueue, "sink");
    g_assert(pad);

    GstPad *video_ghostpad = gst_ghost_pad_new("videosink", pad);
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
