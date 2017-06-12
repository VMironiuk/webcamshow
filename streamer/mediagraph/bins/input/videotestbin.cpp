#include "videotestbin.h"

#include <gst/gst.h>

const char *VideoTestBin::BIN_NAME = "videotestbin";
const char *VideoTestBin::SRC_NAME = "videotestsrc";
const char *VideoTestBin::SRC_ALIAS = "videosrc";
const char *VideoTestBin::GHOST_PAD_NAME = "videotestbinsrc";

VideoTestBin::VideoTestBin()
    : AbstractBin()
{

}

VideoTestBin::~VideoTestBin()
{

}

GstElement *VideoTestBin::get()
{
    // Create elements
    GstElement *bin = gst_bin_new(BIN_NAME);
    if (!bin) {
        g_printerr("VideoTestBin: ERROR: failed to create element of type 'bin'\n");
        return NULL;
    }

    GstElement *videosrc = gst_element_factory_make(SRC_NAME, SRC_ALIAS);
    if (!videosrc) {
        g_printerr("VideoTestBin: ERROR: failed to create element of type '%s'\n", SRC_NAME);
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_bin_add(GST_BIN(bin), videosrc)) {
        g_printerr("VideoTestBin: ERROR: bin doesn't want to accept element of type '%s'\n", SRC_NAME);
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    GstPad *pad = gst_element_get_static_pad(videosrc, "src");
    g_assert(pad);

    GstPad *ghostpad = gst_ghost_pad_new(GHOST_PAD_NAME, pad);
    if (!ghostpad) {
        g_printerr("VideoTestBin: ERROR: failed to create element of type 'ghostpad'\n");
        gst_object_unref(GST_OBJECT(pad));
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_element_add_pad(bin, ghostpad)) {
        g_printerr("VideoTestBin: ERROR: failed while adding ghost pad. Pad with the "
                   "same name already existed or the pad already had another parent\n");
        gst_object_unref(GST_OBJECT(ghostpad));
        gst_object_unref(GST_OBJECT(pad));
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    gst_object_unref(GST_OBJECT(pad));

    return bin;
}
