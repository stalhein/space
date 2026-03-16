# Barnes-Hut Space Simulation
A 3D N-body gravity simulation, made in C++ and OpenGL. The programme visualises the Barnes-Hut octrees as the bodies experience gravity.

## Features
* **Dynamic Octree Visualisation** Space is split into octrees until only one body remains in each node, and the octrees are rendered.
* **N-Body Physics** Gravity is simulated upon bodies, making them move realistically.

## Prerequisits
To build and run the programme, you will need: 
* A C++ compiler (C++17)
* CMake (v3.16 or higher)
* OpenGL drivers

## Build and run instructions
1. **Generate build files and get dependencies:**
    ```bash
    cmake -B build
    ```
2. **Compile and run:**
    ```bash
    cmake --build build --target run
    ```

## Controls
* **Mouse:** Look around
* **W/S:** Forwards/backwards
* **A/D:** Left/right
* **SPACE/LEFT SHIFT:** Up/down
* **Esc:** Exit the programme

## License
This programme is licensed under the [LICENSE](MIT License).
