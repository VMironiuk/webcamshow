#ifndef ABSTRACTGRAPH_H
#define ABSTRACTGRAPH_H

class AbstractGraph
{
public:
    virtual ~AbstractGraph();

    virtual bool start() = 0;
    virtual void stop() = 0;
};

#endif // ABSTRACTGRAPH_H
