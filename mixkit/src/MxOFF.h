#ifndef MXOFF_INCLUDED // -*- C++ -*-
#define MXOFF_INCLUDED

#include "MxStdModel.h"
#include <iostream>

class MxOFFReader
{
public:
    MxOFFReader() {}
    MxStdModel *read(std::istream& in, MxStdModel *model = NULL);
};

#endif
