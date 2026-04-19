import subprocess
import argparse
import sys
import os
import math

def create_cube(filename):
    cube = """OFF
8 12 0
-1 -1 -1
 1 -1 -1
 1  1 -1
-1  1 -1
-1 -1  1
 1 -1  1
 1  1  1
-1  1  1
3 0 1 2
3 0 2 3
3 4 5 6
3 4 6 7
3 0 1 5
3 0 5 4
3 1 2 6
3 1 6 5
3 2 3 7
3 2 7 6
3 3 0 4
3 3 4 7
"""
    with open(filename, 'w') as f:
        f.write(cube)

def create_plane(filename, res=10):
    """Creates a tessellated plane."""
    with open(filename, 'w') as f:
        f.write("OFF\n")
        f.write(f"{(res+1)*(res+1)} {res*res*2} 0\n")
        for j in range(res + 1):
            for i in range(res + 1):
                f.write(f"{i/res} {j/res} 0\n")
        for j in range(res):
            for i in range(res):
                p1 = j * (res + 1) + i
                p2 = p1 + 1
                p3 = (j + 1) * (res + 1) + i
                p4 = p3 + 1
                f.write(f"3 {p1} {p2} {p4}\n")
                f.write(f"3 {p1} {p4} {p3}\n")

def create_sphere(filename, rings=20, sectors=20):
    """Generates a UV sphere."""
    verts = []
    faces = []
    R = 1.0 / (rings - 1)
    S = 1.0 / (sectors - 1)

    for r in range(rings):
        for s in range(sectors):
            y = math.sin(-math.pi / 2 + math.pi * r * R)
            x = math.cos(2 * math.pi * s * S) * math.sin(math.pi * r * R)
            z = math.sin(2 * math.pi * s * S) * math.sin(math.pi * r * R)
            verts.append((x, y, z))

    for r in range(rings - 1):
        for s in range(sectors - 1):
            i0 = r * sectors + s
            i1 = r * sectors + (s + 1)
            i2 = (r + 1) * sectors + (s + 1)
            i3 = (r + 1) * sectors + s
            faces.append((i0, i1, i2))
            faces.append((i0, i2, i3))

    with open(filename, 'w') as f:
        f.write("OFF\n")
        f.write(f"{len(verts)} {len(faces)} 0\n")
        for v in verts:
            f.write(f"{v[0]} {v[1]} {v[2]}\n")
        for fc in faces:
            f.write(f"3 {fc[0]} {fc[1]} {fc[2]}\n")

def run_test(qslim_bin, input_file, target_faces):
    output_file = "out.smf"
    cmd = [qslim_bin, "-t", str(target_faces), "-o", output_file, input_file]
    print(f"Testing {input_file:15} | Target: {target_faces:4} faces... ", end="")
    sys.stdout.flush()
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print("FAILED")
        print(result.stderr)
        return False

    if not os.path.exists(output_file):
        print("MISSING OUTPUT")
        return False

    with open(output_file, 'r') as f:
        lines = f.readlines()
        faces = [l for l in lines if l.startswith('f ')]
        print(f"DONE (Result: {len(faces)} faces)")
    
    os.remove(output_file)
    return True

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--qslim', required=True)
    args = parser.parse_args()

    # Define test cases: (filename, creator_func, target_faces)
    tests = [
        ("test_cube.off", lambda f: create_cube(f), 4),
        ("test_plane.off", lambda f: create_plane(f, res=20), 50),
        ("test_sphere.off", lambda f: create_sphere(f, rings=30, sectors=30), 100),
    ]

    success = True
    for filename, creator, target in tests:
        creator(filename)
        if not run_test(args.qslim, filename, target):
            success = False
        if os.path.exists(filename):
            os.remove(filename)

    if success:
        print("\nAll primitives (Cube, Plane, Sphere) tested successfully.")
    else:
        print("\nPrimitive testing failed.")
        sys.exit(1)

if __name__ == "__main__":
    main()
