#ifndef ABSTRACTBIN_H
#define ABSTRACTBIN_H

typedef struct _GstElement GstElement;
class AbstractBin
{
public:
    virtual ~AbstractBin();
    virtual GstElement *get() = 0;
};

#endif // ABSTRACTBIN_H
