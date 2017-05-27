#include "streamer.h"

#include <gst/gst.h>

int main(int argc, char *argv[])
{
    gst_init(&argc, &argv);

    Streamer streamer(argc, argv);

    return streamer.exec();
}
