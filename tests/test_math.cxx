#include <stdmix.h>
#include <MxVec3.h>
#include <MxMat4.h>
#include <cassert>
#include <iostream>

void test_vec3() {
    Vec3 v1(1, 0, 0);
    Vec3 v2(0, 1, 0);
    
    // Cross product
    Vec3 v3 = v1 ^ v2;
    assert(v3[0] == 0 && v3[1] == 0 && v3[2] == 1);
    
    // Dot product
    double d = v1 * v2;
    assert(d == 0.0);
    
    // Addition
    Vec3 v4 = v1 + v2;
    assert(v4[0] == 1 && v4[1] == 1 && v4[2] == 0);

    std::cout << "Vec3 tests passed." << std::endl;
}

void test_mat4() {
    // Identity matrix should be available as Mat4::I
    Mat4 m = Mat4::I;
    Vec3 v(1, 2, 3);
    Vec3 v_out = m * v;
    
    assert(v_out[0] == 1 && v_out[1] == 2 && v_out[2] == 3);
    
    // Scaling
    Mat4 s = Mat4::scale(2, 2, 2);
    v_out = s * v;
    assert(v_out[0] == 2 && v_out[1] == 4 && v_out[2] == 6);

    std::cout << "Mat4 tests passed." << std::endl;
}

int main() {
    test_vec3();
    test_mat4();
    return 0;
}
