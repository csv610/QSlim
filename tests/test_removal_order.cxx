#include <stdmix.h>
#include <MxStdModel.h>
#include <MxQSlim.h>
#include <cassert>
#include <iostream>
#include <vector>

void test_removal_order() {
    // Create a cube (12 faces, 8 vertices)
    MxStdModel m(8, 12);
    
    // Vertices
    m.add_vertex(0,0,0); m.add_vertex(1,0,0); m.add_vertex(1,1,0); m.add_vertex(0,1,0);
    m.add_vertex(0,0,1); m.add_vertex(1,0,1); m.add_vertex(1,1,1); m.add_vertex(0,1,1);
    
    // Faces
    m.add_face(0,1,2); m.add_face(0,2,3);
    m.add_face(4,5,6); m.add_face(4,6,7);
    m.add_face(0,1,5); m.add_face(0,5,4);
    m.add_face(1,2,6); m.add_face(1,6,5);
    m.add_face(2,3,7); m.add_face(2,7,6);
    m.add_face(3,0,4); m.add_face(3,4,7);

    MxEdgeQSlim slim(m);
    slim.initialize();
    
    // Simplify to 4 faces
    std::vector<MxVertexID> removed = slim.get_removal_order(4);
    
    std::cout << "Removed vertices count: " << removed.size() << std::endl;
    for(size_t i=0; i<removed.size(); i++) {
        std::cout << "  Collapsed vertex: " << removed[i] << std::endl;
        // The vertex should be invalid now in the model
        assert(!m.vertex_is_valid(removed[i]));
    }
    
    assert(removed.size() > 0);
    std::cout << "Removal order test passed." << std::endl;
}

int main() {
    test_removal_order();
    return 0;
}
