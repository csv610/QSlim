#include "stdmix.h"
#include "MxSTL.h"
#include <vector>

MxStdModel *MxSTLReader::read(std::istream& in, MxStdModel *m)
{
    char header[80];
    in.read(header, 80);
    
    uint32_t ntri;
    in.read((char*)&ntri, 4);

    if (!m) m = new MxStdModel(ntri * 3, ntri);

    for (uint32_t i = 0; i < ntri; i++) {
        float n[3], v1[3], v2[3], v3[3];
        in.read((char*)n, 12);
        in.read((char*)v1, 12);
        in.read((char*)v2, 12);
        in.read((char*)v3, 12);
        
        uint16_t attr;
        in.read((char*)&attr, 2);

        uint i1 = m->add_vertex(v1[0], v1[1], v1[2]);
        uint i2 = m->add_vertex(v2[0], v2[1], v2[2]);
        uint i3 = m->add_vertex(v3[0], v3[1], v3[2]);
        m->add_face(i1, i2, i3);
    }

    return m;
}
