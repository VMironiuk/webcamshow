#include "audiotestbin.h"

#include <gst/gst.h>

AudioTestBin::AudioTestBin()
    : AbstractBin()
{

}

AudioTestBin::~AudioTestBin()
{

}

GstElement *AudioTestBin::get()
{
    // Create elements
    GstElement *bin = gst_bin_new("audiotestbin");
    if (!bin) {
        g_printerr("AudioTestBin: ERROR: failed to create element of type 'bin'\n");
        return NULL;
    }

    GstElement *audiosrc = gst_element_factory_make("audiotestsrc", "audiosrc");
    if (!audiosrc) {
        g_printerr("AudioTestBin: ERROR: failed to create element of type 'audiotestsrc'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_bin_add(GST_BIN(bin), audiosrc)) {
        g_printerr("AudioTestBin: ERROR: bin doesn't want to accept element of type 'audiotestsrc'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    GstPad *pad = gst_element_get_static_pad(audiosrc, "src");
    g_assert(pad);

    GstPad *ghostpad = gst_ghost_pad_new("audiotestbinsrc", pad);
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
