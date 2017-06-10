#ifndef AUDIOINPUTBIN_H
#define AUDIOINPUTBIN_H

#include "abstractbin.h"

class AudioInputBin : public AbstractBin
{
public:
    AudioInputBin();
    ~AudioInputBin();
    GstElement *get();
};

#endif // AUDIOINPUTBIN_H
