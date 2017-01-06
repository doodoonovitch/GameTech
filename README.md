Welcome to Gametech!
===================


This is some codes I write to learn 3D graphics programming in OpenGL. I write it like a game engine but for the moment, there is only a 3D graphics stuffs.

> **Note:**
> - to run the test sample, you need to set the folder "Data" as the working folder.
> - from Visual Studio, you can set the property "Debugging" of GameTest project with **$(SolutionDir)Data**


CoreFx : Graphics engine part
-------------

### Main features

- Deferred rendering
- Physically based rendering (PBR) with point lights, directional lights and spot lights
- HDR (with basic Reinhard tone mapping)
- SSAO
- Skydome rendering with atmosphere scattering
- Skybox rendering with a cube map
- Water rendering using vertex displacement mapping and noise map
- Terrain using vertex displacement mapping with tessellation
- Model (mesh) loading (using ASSIM library) and rendering
- 2D text rendering using Freetype library to generate glyphs from true type font

### Other features

- forward rendering for debug objects
- wireframe rendering (for debug purpose)
- deferred buffers visualisation
- solar position calculation (from latitude, longintude and date-time)
- Dual-Quaternion

CoreGame : game engine part
-------------

- OpenGL initialization in window mode or fullscreen with monitor selection


Other 
-------------

#### C++ introspection library

