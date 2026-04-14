# Histograms-Binarization

C++ / OpenCV project implementing **basic histogram operations** (stretching & equalization) and several **image binarization (thresholding) methods**. The program loads an image, shows the original, and lets you run operations from an interactive menu while displaying results in separate OpenCV windows.

## Features

### Histogram operations
- **Histogram calculation** (per RGB channel: min/max and counts)
- **Histogram stretch** (contrast stretching)
- **Histogram equalization** (called “normalize” in code)

### Binarization / thresholding methods
- **Grayscale conversion** (RGB → gray using 0.299R + 0.587G + 0.114B)
- **User threshold** (manual threshold `T`)
- **Percentage black** (chooses `T` so that a chosen % of pixels become black)
- **Mean iterative** threshold selection
- **Entropy selection** threshold selection
- **Minimum error** threshold selection
- **Fuzzy minimum** (fuzzy / entropy-based approach)

The “Compare all binarization methods” option also saves output images (JPG) into the current working directory.

## Project structure

- `main.cpp` – CLI menu program; OpenCV I/O and display; calls histogram + binarization functions
- `histogram.h / histogram.cpp` – `Histogram` class and operations (stretch/equalize/update)
- `binarization.cpp` – grayscale + binarization algorithms
- `utils.h / utils.cpp` – PPM / Pixel utilities used by histogram & binarization code
- `CMakeLists.txt` – CMake build config (OpenCV required)
- `ppm-test-06-p6.ppm` – sample PPM file

## Requirements

- **C++17**
- **OpenCV** (the project uses `opencv2/opencv.hpp`, `highgui`, `imgproc`)
- CMake >= 3.10

## Build (CMake)

### 1) Configure OpenCV path
In `CMakeLists.txt` the project currently uses:

```cmake
set(OpenCV_DIR "C:/opencv/build")
```

Update this to match your system:
- **Windows:** something like `C:/opencv/build`
- **Linux/macOS:** you may be able to remove that line if OpenCV is installed system-wide and CMake can find it.

### 2) Build commands

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

This produces an executable named (per `CMakeLists.txt`) `projekt5`.

## Run

Run the program and enter a path to an image file when prompted:

```bash
./projekt5
```

Then use the menu to:
- compute/display histogram information
- run histogram stretch / equalization
- convert to grayscale
- run different binarization methods
- compare all binarization methods (also saves images like `01_Grayscale.jpg`, `02_UserThreshold_T127.jpg`, etc.)

## Notes / known limitations

- The UI is **console-driven** (menu in terminal) while images are displayed via **OpenCV windows**.
- Some naming in CMake (`project(projekt2_opencv)` / executable `projekt5`) doesn’t match the repo name—feel free to rename for clarity.
- Input/output handling is primarily designed for interactive local use.

