#include "abstractgraph.h"

#include <cstdlib>
#include <iostream>

AbstractGraph::~AbstractGraph()
{

}

void AbstractGraph::error(const char *msg)
{
    std::cerr << msg;
    cleanup();
    std::exit(EXIT_FAILURE);
}
