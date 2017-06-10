#ifndef AUDIOTESTBIN_H
#define AUDIOTESTBIN_H

#include "abstractbin.h"

class AudioTestBin : public AbstractBin
{
public:
    AudioTestBin();
    ~AudioTestBin();
    GstElement *get();
};

#endif // AUDIOTESTBIN_H
