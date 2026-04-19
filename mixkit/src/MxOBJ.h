#ifndef MXOBJ_INCLUDED // -*- C++ -*-
#define MXOBJ_INCLUDED

#include "MxStdModel.h"
#include <iostream>

class MxOBJReader
{
public:
    MxOBJReader() {}
    MxStdModel *read(std::istream& in, MxStdModel *model = NULL);
};

#endif
