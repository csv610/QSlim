/************************************************************************

  MxLineModel

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxLineModel.cxx,v 1.2 1998/10/26 21:09:02 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxLineModel.h"


MxVertexID MxLineModel::add_vertex(float x, float y)
{
    MxVertexID id = vertices.add(MxVertex2D(x,y));
    return id;
}

MxFaceID MxLineModel::add_segment(MxVertexID v1, MxVertexID v2)
{
    MxFaceID id = segments.add(MxSegment2D(v1, v2));
    return id;
}
