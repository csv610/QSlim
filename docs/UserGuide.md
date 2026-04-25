# QSlim: A Comprehensive Guide to Mesh Simplification
**A Manual for Students and Researchers**

---

## Table of Contents
1. [Introduction](#1-introduction)
2. [Core Concepts](#2-core-concepts)
    - [The Quadric Error Metric (QEM)](#the-quadric-error-metric-qem)
    - [Edge Contraction](#edge-contraction)
3. [Installation and Setup](#3-installation-and-setup)
4. [Command-Line Interface](#4-command-line-interface)
    - [Basic Usage](#basic-usage)
    - [Output Formats](#output-formats)
5. [Advanced Control and Optimization](#5-advanced-control-and-optimization)
    - [Placement Policies](#placement-policies)
    - [Weighting Policies](#weighting-policies)
    - [Boundary Preservation](#boundary-preservation)
6. [Mesh Processing Tools](#6-mesh-processing-tools)
7. [Practical Examples](#7-practical-examples)
8. [References](#8-references)

---

## 1. Introduction
Welcome to the world of geometric simplification! **QSlim** is a powerful tool designed to reduce the complexity of 3D polygonal meshes while preserving their visual appearance as closely as possible. 

In computer graphics and computational geometry, high-resolution models often contain millions of triangles. While these provide immense detail, they are computationally expensive to render, transmit, and store. QSlim solves this by "decimating" the mesh—carefully removing vertices and faces to create a lighter version of the original model.

This modernized version of QSlim is built for performance and portability, providing a clean, headless engine for mathematical mesh processing.

---

## 2. Core Concepts

### The Quadric Error Metric (QEM)
The heart of QSlim is the **Quadric Error Metric**, developed by Michael Garland and Paul Heckbert. Instead of using simple heuristics to decide which parts of a mesh to remove, QSlim uses a mathematical model to estimate the "error" or geometric deviation that would be introduced by a simplification step.

Each vertex in the mesh is associated with a 4x4 symmetric matrix (the quadric) that represents the sum of squared distances to the planes of the faces meeting at that vertex. When two vertices are merged, their quadrics are added together. This allows the algorithm to quickly calculate the cost of merging any two vertices and find the optimal position for the new, merged vertex.

### Edge Contraction
QSlim primarily simplifies meshes using **edge contraction** (notated as $(v_1, v_2) \to \bar{v}$). This operation takes two endpoints of an edge, $v_1$ and $v_2$, and collapses them into a single new vertex $\bar{v}$. All triangles previously connected to $v_1$ or $v_2$ are updated to connect to $\bar{v}$, and any degenerate triangles (those that become a line or a point) are removed.

---

## 3. Installation and Setup
QSlim uses the **CMake** build system, making it compatible with Windows, macOS, and Linux.

### Prerequisites
- A C++17 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- CMake 3.10 or higher

### Building from Source
```bash
# 1. Create a build directory
mkdir build && cd build

# 2. Configure the project
cmake ..

# 3. Compile the software
make
```

After compilation, you will find the `qslim` executable in `tools/qslim/`.

---

## 4. Command-Line Interface

### Basic Usage
The most common task is simplifying a model to a target face count. 

```bash
qslim -t <target_faces> -o <output_file> <input_file>
```

**Example:**
To simplify a high-poly "bunny.obj" model down to 5,000 faces:
```bash
./qslim -t 5000 -o simplified_bunny.obj bunny.obj
```

### Output Formats
QSlim supports several industry-standard formats. You can specify the output format using the `-M` flag or by using the appropriate file extension with `-o`.

Supported formats include:
- **OBJ**: Wavefront Object (Universal support)
- **OFF**: Object File Format (Common in research)
- **STL**: Stereolithography (Standard for 3D printing)
- **SMF**: Simple Model Format (Native format for QSlim)
- **VRML/IV**: Virtual Reality Modeling Language / OpenInventor

---

## 5. Advanced Control and Optimization

QSlim provides fine-grained control over the simplification process through various "policies."

### Placement Policies (`-O <n>`)
This determines where the new vertex $\bar{v}$ will be placed after a contraction.
- `-O 0`: **Endpoints**. Only considers $v_1$ or $v_2$ as the new position.
- `-O 1`: **End or Midpoint**. Considers $v_1$, $v_2$, or the average $(\frac{v_1+v_2}{2})$.
- `-O 3`: **Optimal [Default]**. Mathematically calculates the position that minimizes the quadric error.

### Weighting Policies (`-W <n>`)
Determines how much "importance" is given to different triangles.
- `-W 0`: **Uniform**. All triangles are treated equally.
- `-W 1`: **Area [Default]**. Larger triangles have more influence on the quadric.
- `-W 2`: **Angle**. Weights based on the angles of the triangles.

### Boundary Preservation (`-B <weight>`)
If your mesh has "open edges" (holes or boundaries), they might shrink or distort during simplification. Using the `-B` flag adds extra quadric constraints to these edges to keep them in place. A typical weight is `1000.0`.

---

## 6. Mesh Processing Tools
In addition to the main `qslim` tool, the suite includes several "filters" for pre- and post-processing:

- **smfclean**: Removes duplicate vertices and degenerate faces. Always a good idea to run this before simplification!
- **smfmeasure**: Compares two meshes and calculates the geometric error between them.
- **smf2ply**: Converts models to the PLY format.
- **smfcat**: Combines multiple mesh files into a single scene.

---

## 7. Practical Examples

### Case 1: Preparing a model for 3D Printing
3D scans are often extremely dense. To make them manageable for "slicing" software:
```bash
./qslim -t 50000 -O 3 -B 1000 -o printable_scan.stl raw_scan.obj
```

### Case 2: Generating Level of Detail (LOD) for Games
You might want a 75%, 50%, and 25% version of your character:
```bash
# 50% simplification
./qslim -t 10000 -o lod_mid.obj high_poly.obj
# 90% simplification
./qslim -t 2000 -o lod_low.obj high_poly.obj
```

---

## 8. References
For students interested in the underlying mathematics, the original papers are highly recommended:

1. Garland, Michael, and Paul S. Heckbert. "**Surface simplification using quadric error metrics.**" *Proceedings of the 24th annual conference on Computer graphics and interactive techniques*. 1997.
2. Garland, Michael. "**Quadric-based polygonal surface simplification.**" *PhD thesis, Carnegie Mellon University*. 1999.

---
*Manual compiled for the Modernized QSlim Project.*
