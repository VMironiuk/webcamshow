#ifndef STREAMER_H
#define STREAMER_H

class AbstractGraph;
class Streamer
{
public:
    Streamer(int argc, char **argv);
    ~Streamer();
    int exec();

private:
    AbstractGraph *m_graph;
};

#endif // STREAMER_H
