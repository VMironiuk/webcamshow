#include "abstractoutputbin.h"

#include <iostream>
#include <cstdlib>

AbstractOutputBin::~AbstractOutputBin()
{

}

void AbstractOutputBin::error(const char *msg)
{
    std::cerr << msg;
    std::exit(1);
}
