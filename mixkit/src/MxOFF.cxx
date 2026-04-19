#include "stdmix.h"
#include "MxOFF.h"
#include <string>

MxStdModel *MxOFFReader::read(std::istream& in, MxStdModel *m)
{
    std::string header;
    in >> header;
    if (header != "OFF") {
        mxmsg_signal(MXMSG_FATAL, "Not a valid OFF file", NULL);
        return NULL;
    }

    uint nv, nf, ne;
    if (!(in >> nv >> nf >> ne)) {
        mxmsg_signal(MXMSG_FATAL, "Failed to read OFF counts", NULL);
        return NULL;
    }

    if (!m) m = new MxStdModel(nv, nf);

    for (uint i = 0; i < nv; i++) {
        float x, y, z;
        in >> x >> y >> z;
        m->add_vertex(x, y, z);
    }

    for (uint i = 0; i < nf; i++) {
        uint n;
        in >> n;
        if (n < 3) {
            // Skip invalid faces
            for (uint j = 0; j < n; j++) { uint dummy; in >> dummy; }
            continue;
        }

        uint vfirst, vprev, vcurr;
        in >> vfirst >> vprev;
        for (uint j = 2; j < n; j++) {
            in >> vcurr;
            m->add_face(vfirst, vprev, vcurr);
            vprev = vcurr;
        }
    }

    return m;
}
