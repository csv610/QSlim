import subprocess
import argparse
import sys
import os

def create_cube_smf_obj(filename):
    cube = """v 0 0 0
v 1 0 0
v 1 1 0
v 0 1 0
v 0 0 1
v 1 0 1
v 1 1 1
v 0 1 1
f 1 2 3
f 1 3 4
f 5 6 7
f 5 7 8
f 1 2 6
f 1 6 5
f 2 3 7
f 2 7 6
f 3 4 8
f 3 8 7
f 4 1 5
f 4 5 8
"""
    with open(filename, 'w') as f:
        f.write(cube)

def create_cube_off(filename):
    cube = """OFF
8 12 0
0 0 0
1 0 0
1 1 0
0 1 0
0 0 1
1 0 1
1 1 1
0 1 1
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

def run_test(qslim_bin, input_file, target_faces=4):
    output_smf = "out.smf"
    cmd = [qslim_bin, "-t", str(target_faces), "-o", output_smf, input_file]
    print(f"Running: {' '.join(cmd)}")
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"QSlim failed for {input_file}!")
        print(result.stderr)
        return False

    if not os.path.exists(output_smf):
        print(f"Output file not created for {input_file}!")
        return False

    with open(output_smf, 'r') as f:
        lines = f.readlines()
        faces = [l for l in lines if l.startswith('f ')]
        print(f"  Input: {input_file}, Target: {target_faces}, Result faces: {len(faces)}")
        if len(faces) == 0:
            print("  Error: Model vanished!")
            return False
    
    os.remove(output_smf)
    return True

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--qslim', required=True)
    args = parser.parse_args()

    formats = [
        ("cube.smf", create_cube_smf_obj),
        ("cube.obj", create_cube_smf_obj),
        ("cube.off", create_cube_off),
    ]

    success = True
    for filename, creator in formats:
        creator(filename)
        if not run_test(args.qslim, filename):
            success = False
        os.remove(filename)

    if success:
        print("All CLI Integration tests passed.")
    else:
        print("Some tests failed.")
        sys.exit(1)

if __name__ == "__main__":
    main()
