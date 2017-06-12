#ifndef AUDIOTESTBIN_H
#define AUDIOTESTBIN_H

#include "abstractbin.h"

class AudioTestBin : public AbstractBin
{
public:
    AudioTestBin();
    ~AudioTestBin();
    GstElement *get();

    static const char *BIN_NAME;
    static const char *SRC_NAME;
    static const char *SRC_ALIAS;
    static const char *GHOST_PAD_NAME;
};

#endif // AUDIOTESTBIN_H
