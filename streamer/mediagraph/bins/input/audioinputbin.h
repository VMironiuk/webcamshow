#ifndef AUDIOINPUTBIN_H
#define AUDIOINPUTBIN_H

#include "abstractbin.h"

class AudioInputBin : public AbstractBin
{
public:
    AudioInputBin();
    ~AudioInputBin();
    GstElement *get();

    static const char *BIN_NAME;
    static const char *SRC_NAME;
    static const char *SRC_ALIAS;
    static const char *GHOST_PAD_NAME;
};

#endif // AUDIOINPUTBIN_H
