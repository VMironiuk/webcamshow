#ifndef ABSTRACTOUTPUTBIN_H
#define ABSTRACTOUTPUTBIN_H

typedef struct _GstElement GstElement;
class AbstractOutputBin
{
public:
    virtual ~AbstractOutputBin();
    virtual GstElement *get() = 0;
};

#endif // ABSTRACTOUTPUTBIN_H
