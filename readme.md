# Raytracer

## Build & Run
The project is implemented in C++20, using:
* CMake > 3.15 - build system
* Docker - reproducible build environment
* Python 3 - Build and run orchestration scripts

To avoid manual dependency installation on the host system, the entire build process is encapsulated inside a Docker image.

### Building the application
To build the project, run:
```bash
./tools/build.py
```

The script will:
* automatically setup docker image with all of the necessary dependencies
* compile the code inside the container and export it to the *build* directory on host machine

You can also compile the project manually.
Having `build-essential` and your C++20 compiler of choice installed, run:
```bash
mkdir build
cd build
cmake ../src
make
cd ..
```

### Running the application
After a successful build, the application can be launched with:
```bash
./tools/run.py <config.json path>
```

or manually:
```bash
./build/raytracer config/suzanne-scene.json
```

## Sources
1. https://raytracing.github.io/books/RayTracingInOneWeekend.html