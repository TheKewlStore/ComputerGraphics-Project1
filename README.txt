Project 1 -- Ian Davis

The project was written using CLion IDE Eduational Edition.
The code is contained in main.cpp.

Basic usage: ./main {path_to_ply_file}

The main function does all one-time setup work.
It parses the command line for a ply filepath,
reads the ply file,
sets up the glut window,
and enters the glut main loop.

The draw function is the main renderer.
It handles clearing the color and depth buffers,
Setting up the three viewports,
renders the ply for each viewport,
and renders x y and z axis lines for each viewport.

The render_ply function is in charge of actually rendering all the vertices based on the PLY faces.

The draw_axes function simply draws lines following the x, y and z axes to make visualizing the models easier.

The resize function just handles window resizes
and simply updates the width and height variables and redraws the scene.

CLion uses cmake for building/deploying,
so the CMakeLists.txt contains cmake instructions to build and run this project.
To the best of my knowledge, if cmake is properly installed, this "should" just work.

Documentation for all code is provided in-line and via docstrings.
