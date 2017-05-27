#include "videopreviewbin.h"

#include <gst/gst.h>

#include <cassert>

VideoPreviewBin::VideoPreviewBin(int winid)
    : AbstractOutputBin(),
      m_winid(winid)
{

}

VideoPreviewBin::~VideoPreviewBin()
{

}

GstElement *VideoPreviewBin::get()
{
    GstElement *bin = gst_bin_new("videopreview-bin");
    if (!bin)
        error("VideoPreview: ERROR: failed to create bin\n");

    GstElement *videoqueue = gst_element_factory_make("queue", "videoqueue");
    if (!videoqueue) {
        gst_object_unref(GST_OBJECT(bin));
        error("VideoPreview: ERROR: failed to create queue\n");
    }
    // Leaky queue necessary to work with rtmp streaming
    g_object_set(G_OBJECT(videoqueue), "leaky", 1, NULL);
    gst_bin_add(GST_BIN(bin), videoqueue);

    GstElement *videosink = NULL;
    m_winid == 0
            ? videosink = gst_element_factory_make("autovideosink", "videosink")
            : videosink = gst_element_factory_make("xvimagesink", "videosink");
    if (!videosink) {
        gst_object_unref(GST_OBJECT(bin));
        error("VideoPreview: ERROR: failed to create videosink\n");
    }
    gst_bin_add(GST_BIN(bin), videosink);

    // Set up ghost pad
    GstPad *pad = gst_element_get_static_pad(videoqueue, "sink");
    assert(pad);

    GstPad *ghostpad = gst_ghost_pad_new("videopreviewsink", pad);
    if (!ghostpad) {
        gst_object_unref(GST_OBJECT(pad));
        gst_object_unref(GST_OBJECT(bin));
        error("VideoPreview: ERROR: failed to create ghost pad\n");
    }

    if (!gst_element_add_pad(bin, ghostpad)) {
        gst_object_unref(GST_OBJECT(ghostpad));
        gst_object_unref(GST_OBJECT(pad));
        gst_object_unref(GST_OBJECT(bin));
        error("VideoPreview: ERROR: failed while adding ghost pad. Pad with the "
              "same name already existed or the pad already had another parent\n");
    }

    // Link elements
    if (!gst_element_link(videoqueue, videosink)) {
        gst_object_unref(GST_OBJECT(pad));
        gst_object_unref(GST_OBJECT(bin));
        error("VideoPreview: ERROR: failed to link videoqueue and videosink\n");
    }

    gst_object_unref(GST_OBJECT(pad));

    return bin;
}
