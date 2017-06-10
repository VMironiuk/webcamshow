#include "videopreviewbin.h"

#include <gst/gst.h>

VideoPreviewBin::VideoPreviewBin(int winid)
    : AbstractBin(),
      m_winid(winid)
{

}

VideoPreviewBin::~VideoPreviewBin()
{

}

GstElement *VideoPreviewBin::get()
{
    // Create elements
    GstElement *bin = gst_bin_new("videopreviewbin");
    if (!bin) {
        g_printerr("VideoPreviewBin: ERROR: failed to create element of type 'bin'\n");
        return NULL;
    }

    GstElement *videoqueue = gst_element_factory_make("queue", "videoqueue");
    if (!videoqueue) {
        g_printerr("VideoPreviewBin: ERROR: failed to create element of type 'queue'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }
    // Leaky queue necessary to work with rtmp streaming
    g_object_set(G_OBJECT(videoqueue), "leaky", 1, NULL);

    if (!gst_bin_add(GST_BIN(bin), videoqueue)) {
        g_printerr("VideoPreviewBin: ERROR: bin doesn't want to accept element of type 'videoqueue'\n");
        gst_object_unref(GST_OBJECT(bin));
        gst_object_unref(GST_OBJECT(videoqueue));
        return NULL;
    }

    GstElement *videosink = NULL;
    m_winid == 0
            ? videosink = gst_element_factory_make("autovideosink", "videosink")
            : videosink = gst_element_factory_make("xvimagesink", "videosink");
    if (!videosink) {
        g_printerr("VideoPreviewBin: ERROR: failed to create element of type 'videosink'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_bin_add(GST_BIN(bin), videosink)) {
        g_printerr("VideoPreviewBin: ERROR: bin doesn't want to accept element of type 'videosink'\n");
        gst_object_unref(GST_OBJECT(bin));
        gst_object_unref(GST_OBJECT(videosink));
        return NULL;
    }

    // Set up ghost pad
    GstPad *pad = gst_element_get_static_pad(videoqueue, "sink");
    g_assert(pad);

    GstPad *ghostpad = gst_ghost_pad_new("videopreviewsink", pad);
    if (!ghostpad) {
        g_printerr("VideoPreviewBin: ERROR: failed to create element of type 'ghostpad'\n");
        gst_object_unref(GST_OBJECT(pad));
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_element_add_pad(bin, ghostpad)) {
        g_printerr("VideoPreviewBin: ERROR: failed while adding ghost pad. Pad with the "
                   "same name already existed or the pad already had another parent\n");
        gst_object_unref(GST_OBJECT(ghostpad));
        gst_object_unref(GST_OBJECT(pad));
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    gst_object_unref(GST_OBJECT(pad));

    // Link elements
    if (!gst_element_link(videoqueue, videosink)) {
        g_printerr("VideoPreviewBin: ERROR: failed to link 'videoqueue' and 'videosink'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    return bin;
}
