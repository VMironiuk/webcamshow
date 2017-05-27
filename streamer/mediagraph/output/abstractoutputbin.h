#ifndef ABSTRACTOUTPUTBIN_H
#define ABSTRACTOUTPUTBIN_H

typedef struct _GstElement GstElement;
class AbstractOutputBin
{
public:
    virtual ~AbstractOutputBin();
    virtual GstElement *get() = 0;

protected:
    void error(const char *msg);
};

#endif // ABSTRACTOUTPUTBIN_H
