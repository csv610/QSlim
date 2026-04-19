# QSlim 2.0 (Modernized)

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)
[![Build Status](https://img.shields.io/badge/Build-CMake-success.svg)](#building)

A modernized, high-performance, and headless implementation of the classic **QSlim** surface simplification software.

This version is a "clean-room" modernization of the original 1999 codebase. It strips away legacy 90s-era GUI dependencies (OpenGL/X11) to provide a pure mathematical engine for mesh decimation, updated for modern C++ standards and build pipelines.

---

## 🚀 Key Modernization Features

*   **Zero Dependencies:** Completely removed OpenGL, X11, and XForms. It can now be built on any headless server or container without a GPU or display driver.
*   **C++17 Standard:** Refactored for modern compilers. This includes:
    *   **Smart Pointers (RAII):** Replaced manual memory management with `std::unique_ptr` to prevent leaks.
    *   **Type Safety:** Systematically replaced legacy `NULL` with `nullptr` and added `override` keywords.
    *   **Modern Iterators:** Added support for range-based `for` loops (`for (auto& x : block)`) to core data structures.
    *   **Move Semantics:** Implemented move constructors and assignment for efficient data transfer.
*   **New API Capabilities:**
    *   `get_removal_order()`: Returns the exact sequence of vertices removed during decimation.
    *   `get_protected_vertices()`: Automatically identifies critical boundary and non-manifold vertices that should be preserved.
*   **Modern Build System:** Replaced fragile legacy Makefiles with a robust **CMake** configuration.
*   **Multi-Format Support:** Added native support for modern 3D formats:
    *   **OBJ** (Alias Wavefront)
    *   **OFF** (Object File Format)
    *   **STL** (Binary Stereolithography)
    *   **SMF** (Original Michael Garland format)
*   **100% Warning-Free:** The codebase now compiles with zero warnings under strict modern standards.
*   **Automated Verification:** Integrated `ctest` suite ensuring mathematical correctness across all file formats.

---

## ⚡ Performance & Scalability

The modernized codebase has been stress-tested with high-resolution meshes to ensure stability and efficiency.

| Mesh Type | Original Faces | Target Faces | Time (Approx.) | Status |
| :--- | :--- | :--- | :--- | :--- |
| **Cube** | 12 | 4 | < 0.01s | ✅ Pass |
| **Tessellated Plane** | 5,000 | 100 | 0.05s | ✅ Pass |
| **High-Res Sphere** | 50,000 | 200 | 0.45s | ✅ Pass |
| **Stress Test Sphere** | **500,000** | **1,000** | **5.3s** | ✅ Pass |

The adoption of **STL-backed containers** and **C++17 optimizations** allows for rapid simplification of large-scale models (up to 99.8% reduction) with minimal memory overhead.

---

## 🧠 Background: Why QSlim?

Developed by **Michael Garland** and **Paul Heckbert** at Carnegie Mellon University, the Quadric Error Metric (QEM) algorithm is the "gold standard" for mesh simplification. It provides the optimal balance between geometric fidelity and computational speed. 

While the original software was a research prototype, this modernized version makes it production-ready for:
*   **Game Development:** Automatic LOD (Level of Detail) generation.
*   **Web 3D:** Compressing massive models for browser-based rendering.
*   **3D Printing:** Decimating high-poly scans for slicer compatibility.

---

## 🛠 Building

### Prerequisites
*   CMake 3.10 or higher
*   A C++17 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)

### Compilation
```bash
# Clone the repository
git clone https://github.com/csv610/QSlim.git
cd QSlim

# Build
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)

# Run Tests
ctest
```

---

## 💻 Usage

The core tool is the `qslim` command-line utility.

### Basic Simplification
Simplify a model to a target number of faces (e.g., 5,000 faces):
```bash
./tools/qslim/qslim -t 5000 -o output.obj input.obj
```

### Key Options
| Flag | Description |
| :--- | :--- |
| `-t <n>` | Set the target number of faces. |
| `-o <file>` | Specify output filename (supports .obj, .smf). |
| `-O <0-3>` | Optimal placement policy (3 = Default Optimal). |
| `-W <0-2>` | Quadric weighting (1 = Area Weighted [Default]). |
| `-B <weight>` | Use boundary preservation planes (important for open meshes). |
| `-q` | Quiet mode (minimal console output). |

---

## 📦 Included Tools

Beyond `qslim`, the `tools/filters` directory provides several "Lego-block" utilities for mesh processing:
*   **smfclean:** Strips degenerate triangles and isolated vertices.
*   **smfmeasure:** Calculates the geometric error between two models.
*   **smfcat:** Concatenates multiple mesh files into one.
*   **smf2ply / smf2c:** Converters for PLY and C-header data.

---

## 📜 Credits & License

**Original Research & Implementation:**  
Michael Garland and Paul Heckbert.  
*Surface Simplification Using Quadric Error Metrics*, SIGGRAPH 97.

**Modernization Authors:**  
Chaman Singh Verma and Contributors.

**License:**  
This software is distributed under the **GNU General Public License (GPL) v2**. See `COPYING.txt` for the full license text.
