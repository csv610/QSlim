#include "stdmix.h"
#include "MxOBJ.h"
#include <string>
#include <sstream>
#include <vector>

MxStdModel *MxOBJReader::read(std::istream& in, MxStdModel *m)
{
    if (!m) m = new MxStdModel(100, 100);

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            m->add_vertex(x, y, z);
        } else if (type == "f") {
            std::vector<uint> indices;
            std::string vertex_ref;
            while (ss >> vertex_ref) {
                std::stringstream vss(vertex_ref);
                std::string index_str;
                std::getline(vss, index_str, '/');
                int index = std::stoi(index_str);
                if (index < 0) index = m->vert_count() + index;
                else index--; // OBJ is 1-based
                indices.push_back((uint)index);
            }

            if (indices.size() >= 3) {
                for (size_t i = 1; i < indices.size() - 1; i++) {
                    m->add_face(indices[0], indices[i], indices[i+1]);
                }
            }
        }
    }

    return m;
}
