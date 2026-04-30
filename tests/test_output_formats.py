import subprocess
import os
import sys

def run_qslim(qslim_bin, args):
    cmd = [qslim_bin] + args
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result

def run_qslim_to_file(qslim_bin, args, outfile):
    with open(outfile, 'w') as f:
        subprocess.run([qslim_bin] + args, stdout=f)

def test_formats(qslim_bin):
    # Create a simple input file
    input_file = "input.smf"
    with open(input_file, 'w') as f:
        f.write("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n")

    success = True

    # 1. Test default format (should be OFF)
    print("Testing default format (stdout)...")
    out_stdout = "out.stdout"
    run_qslim_to_file(qslim_bin, ["-q", "-t", "1", input_file], out_stdout)
    with open(out_stdout, 'r') as f:
        stdout = f.read().strip()
    
    if stdout.startswith("OFF") or "OFF" in stdout.split('\n')[0]:
        print("  PASS: Default is OFF")
    else:
        print(f"  FAIL: Default is not OFF. Starts with: {stdout[:20]!r}")
        success = False
    if os.path.exists(out_stdout): os.remove(out_stdout)

    # 2. Test inference from extension
    extensions = [
        ("out.obj", "v "),
        ("out.off", "OFF"),
        ("out.ply", "ply"),
        ("out.iv", "#Inventor"),
        ("out.vrml", "#VRML"),
        ("out.pm", "v "), # PM starts with base mesh vertices
    ]

    for ext, marker in extensions:
        print(f"Testing inference for {ext}...")
        if os.path.exists(ext): os.remove(ext)
        res = run_qslim(qslim_bin, ["-o", ext, "-t", "1", input_file])
        if res.returncode != 0:
            print(f"  FAIL: qslim returned {res.returncode}")
            print(res.stderr)
            success = False
            continue
        
        if not os.path.exists(ext):
            print(f"  FAIL: {ext} was not created")
            success = False
            continue

        with open(ext, 'r') as f:
            content = f.read(100)
            if marker in content:
                print(f"  PASS: {ext} has correct marker '{marker}'")
            else:
                print(f"  FAIL: {ext} does not have marker '{marker}'")
                print(f"  Content start: {content[:50]!r}")
                success = False
        os.remove(ext)

    # 3. Test that -M is really gone (should fail as unknown option)
    print("Testing that -M is removed...")
    res = run_qslim(qslim_bin, ["-M", "obj", input_file])
    if res.returncode != 0 and "Malformed command line" in res.stderr:
        print("  PASS: -M is rejected")
    else:
        print(f"  FAIL: -M was not rejected as expected. Return code: {res.returncode}")
        success = False

    os.remove(input_file)
    return success

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python test_output_formats.py <path_to_qslim>")
        sys.exit(1)
    
    if test_formats(sys.argv[1]):
        print("\nAll output format tests passed.")
    else:
        print("\nSome output format tests failed.")
        sys.exit(1)
