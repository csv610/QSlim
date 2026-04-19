# QSlim 2.0 (Modernized)

A modernized version of the classic **QSlim** surface simplification software.

This repository provides a clean, headless, and modern C++17 implementation of the Quadric Error Metric (QEM) simplification algorithm. All legacy GUI/OpenGL dependencies have been removed, making it ideal for command-line processing, server-side automation, and integration into modern 3D pipelines.

## Credits & History

**Original Author:** Michael Garland ([garland@uiuc.edu](mailto:garland@uiuc.edu))  
**Collaborator:** Paul Heckbert  
**Original Release:** 1998-1999 (University of Illinois)

The underlying Quadric Error Metric (QEM) developed by Garland and Heckbert remains the industry standard for geometric simplification and is used today in tools like Unity, Unreal Engine, and Blender.

## Modernization Features (2026)

This version includes several significant updates to the original 1999 codebase:

*   **Modern Build System:** Full **CMake** integration (replaces legacy Makefiles).
*   **C++17 Standard:** Code updated for compatibility with modern compilers (Clang 15+, GCC 11+, MSVC 2019+).
*   **Expanded Format Support:** Native support for **OBJ**, **OFF**, and **STL (Binary)**, in addition to the original SMF format.
*   **Zero Dependencies:** Completely stripped of OpenGL, X11, and XForms. It is now a pure mathematical/CLI toolkit.
*   **Automated Testing:** Integrated `ctest` suite covering core math, data structures, and CLI integration.
*   **Improved Safety:** Replaced unsafe legacy C functions (e.g., `vsprintf`) with safer modern alternatives.

## Building

```bash
mkdir build && cd build
cmake ..
make
```

To run the tests:
```bash
ctest
```

## Usage

The primary tool is `qslim`, located in `build/tools/qslim/qslim`.

```bash
qslim -t <target_face_count> -o output.obj input.obj
```

### Supported Formats
- **Input:** .obj, .off, .stl (binary), .smf
- **Output:** .smf (default), .iv, .vrml

## Filters & Tools
The `tools/filters` directory contains several utility programs:
- `smfclean`: Remove degenerate faces and isolated vertices.
- `smfcat`: Combine multiple SMF files.
- `smfmeasure`: Compute geometric differences between models.
- `smf2ply` / `smf2c`: Convert models to other formats.

## License
This software is distributed under the terms of the **GNU General Public License (GPL)**. See `COPYING.txt` and `GPL.txt` for details.
