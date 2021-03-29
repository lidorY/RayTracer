# RayTracer
Basic ray-tracing study project.
This program renders 3D, code defined, 3D scenes with basic local illumination
models.<br>

For a camera, representing a "human eye", with a configuearble FOV and screen dimensions the software generates an image for defined scene.
This is done by simulating a recursive trace of light rays from the camera throughout the scene, bouncing them around and on or through the surface of objects.
The recursivne depth can be configured from within the code.

Rays are categorized into 3 different types:
  - Intersection detection
  - Shadow feelers for shadowed/diffuse areas deduction
  - reflection rays, enabling reflective surfaces
  - refraction rays, enabling transparent objects

Output image is drawn as a BMP output to the console window.

# Features
  - Pre-defined object classes, i.e. Spheres and Planes
    -  Ray collision definition for each
    -  Normal calculation for a given point
  - Ray tracing based intersection detection
    -  defining object drawing order and overlapping zones
    -  Translating objects mathematical representation into a 2D screen projection
  - Phong Illumination model for diffuse surfaces
    - Light and color simulation of the object surfaces based on point lights position
    - Multiple point light sources support
  - Point light shadows
    - Using intersection detection to affect the surface color, based on full or partial shadow obscure mode.
  - Refraction and Reflection based on Whitted light transport algorithm


<img width="318" alt="result" src="https://user-images.githubusercontent.com/15870496/112654121-21341c80-8e60-11eb-8157-272d5f823cfe.png"> <img width="320" alt="rt3" src="https://user-images.githubusercontent.com/15870496/112795906-e1448380-9071-11eb-87ea-cc593c6f58d6.png">

<img width="318" alt="rt2" src="https://user-images.githubusercontent.com/15870496/112795548-58c5e300-9071-11eb-8331-2141346baa49.png"> <img width="318" alt="rt4" src="https://user-images.githubusercontent.com/15870496/112796262-67f96080-9072-11eb-8532-1f7f5bb8305d.png">


# Building
Compiled and tested on MSVC 2019.

This code requires `<Windows.h>` header file! --> Windows only.

- Create a build folder
- CD into this folder
- Run CMake in the folder using relevant MSVC generator - `cmake .. -G "Visual Studio 16 2019"`

