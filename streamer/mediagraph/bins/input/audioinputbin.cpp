#include "audioinputbin.h"

#include <gst/gst.h>

const char *AudioInputBin::BIN_NAME = "audioinputbin";
const char *AudioInputBin::SRC_NAME = "alsasrc";
const char *AudioInputBin::SRC_ALIAS = "audiosrc";
const char *AudioInputBin::GHOST_PAD_NAME = "audioinputbinsrc";

AudioInputBin::AudioInputBin()
    : AbstractBin()
{

}

AudioInputBin::~AudioInputBin()
{

}

GstElement *AudioInputBin::get()
{
    // Create elements
    GstElement *bin = gst_bin_new(BIN_NAME);
    if (!bin) {
        g_printerr("AudioInputBin: ERROR: failed to create element of type 'bin'\n");
        return NULL;
    }

    GstElement *audiosrc = gst_element_factory_make(SRC_NAME, SRC_ALIAS);
    if (!audiosrc) {
        g_printerr("AudioInputBin: ERROR: failed to create element of type '%s'\n", SRC_NAME);
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }
    g_object_set(G_OBJECT(audiosrc), "device", "hw:1", NULL);

    if (!gst_bin_add(GST_BIN(bin), audiosrc)) {
        g_printerr("AudioInputBin: ERROR: bin doesn't want to accept element of type '%s'\n", SRC_NAME);
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    GstPad *pad = gst_element_get_static_pad(audiosrc, "src");
    g_assert(pad);

    GstPad *ghostpad = gst_ghost_pad_new(GHOST_PAD_NAME, pad);
    if (!ghostpad) {
        g_printerr("AudioInputBin: ERROR: failed to create element of type 'ghostpad'\n");
        gst_object_unref(GST_OBJECT(pad));
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    if (!gst_element_add_pad(bin, ghostpad)) {
        g_printerr("AudioInputBin: ERROR: failed while adding ghost pad. Pad with the "
                   "same name already existed or the pad already had another parent\n");
        gst_object_unref(GST_OBJECT(ghostpad));
        gst_object_unref(GST_OBJECT(pad));
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    gst_object_unref(GST_OBJECT(pad));

    return bin;
}
