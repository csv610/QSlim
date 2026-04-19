#include <stdmix.h>
#include <MxStdModel.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

void test_protected_vertices() {
    // Create a simple mesh with a boundary: two triangles sharing an edge (1-2)
    // 0 -- 1 -- 3
    //  \  / \  /
    //   2     2
    // Vertices 0 and 3 are on the boundary.
    // Actually, in a 2-triangle strip, ALL vertices are on the boundary.
    // 0-1, 0-2 are boundary edges.
    // 1-3, 3-2 are boundary edges.
    // 1-2 is the ONLY internal edge.
    
    MxStdModel m(4, 2);
    m.add_vertex(0, 0, 0); // 0
    m.add_vertex(1, 1, 0); // 1
    m.add_vertex(0, 1, 0); // 2
    m.add_vertex(2, 1, 0); // 3
    
    m.add_face(0, 1, 2);
    m.add_face(1, 3, 2);
    
    std::vector<MxVertexID> protected_verts = m.get_protected_vertices();
    
    std::cout << "Protected vertices count: " << protected_verts.size() << std::endl;
    // For this simple strip, all 4 vertices are part of at least one boundary edge.
    assert(protected_verts.size() == 4);
    
    // Now create a closed loop (minimal: a tetrahedron)
    // All edges in a tetrahedron have 2 faces -> 0 protected vertices
    MxStdModel m2(4, 4);
    m2.add_vertex(0,0,0); m2.add_vertex(1,0,0); m2.add_vertex(0,1,0); m2.add_vertex(0,0,1);
    m2.add_face(0,1,2); m2.add_face(0,2,3); m2.add_face(0,3,1); m2.add_face(1,3,2);
    
    std::vector<MxVertexID> protected_verts2 = m2.get_protected_vertices();
    std::cout << "Tetrahedron protected vertices count: " << protected_verts2.size() << std::endl;
    assert(protected_verts2.size() == 0);

    std::cout << "Protected vertices test passed." << std::endl;
}

int main() {
    test_protected_vertices();
    return 0;
}
