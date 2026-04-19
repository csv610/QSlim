#ifndef MXSTL_INCLUDED // -*- C++ -*-
#define MXSTL_INCLUDED

#include "MxStdModel.h"
#include <iostream>

class MxSTLReader
{
public:
    MxSTLReader() {}
    MxStdModel *read(std::istream& in, MxStdModel *model = NULL);
};

#endif
