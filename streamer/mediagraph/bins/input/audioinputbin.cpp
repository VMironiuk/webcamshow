#include "audioinputbin.h"

#include <gst/gst.h>

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
    GstElement *bin = gst_bin_new("audioinputbin");
    if (!bin) {
        g_printerr("AudioInputBin: ERROR: failed to create element of type 'bin'\n");
        return NULL;
    }

    GstElement *audiosrc = gst_element_factory_make("alsasrc", "audiosrc");
    if (!audiosrc) {
        g_printerr("AudioInputBin: ERROR: failed to create element of type 'alsasrc'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }
    g_object_set(G_OBJECT(audiosrc), "device", "hw:1", NULL);

    if (!gst_bin_add(GST_BIN(bin), audiosrc)) {
        g_printerr("AudioInputBin: ERROR: bin doesn't want to accept element of type 'alsasrc'\n");
        gst_object_unref(GST_OBJECT(bin));
        return NULL;
    }

    GstPad *pad = gst_element_get_static_pad(audiosrc, "src");
    g_assert(pad);

    GstPad *ghostpad = gst_ghost_pad_new("audioinputbinsrc", pad);
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
