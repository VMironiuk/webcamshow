#ifndef ABSTRACTGRAPH_H
#define ABSTRACTGRAPH_H

class AbstractGraph
{
public:
    virtual ~AbstractGraph();

    virtual bool start() = 0;
    virtual void stop() = 0;

protected:
    void error(const char *msg);
    virtual void cleanup() = 0;
};

#endif // ABSTRACTGRAPH_H
