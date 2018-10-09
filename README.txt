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

CHANGELIST - PROJECT 2
For project 2, the main changes were separated out into their own functions to ease readability.
First off, the draw function's code was broken out and repeated code refactored into one function,
draw_viewport, which handles drawing each of the ply viewports individually, applying rotations as necessary.
The draw_viewport function was also updated to use the x, y and z rotation angle global variables to rotate the model
around the respective axes by the given angle.
These global variables is updated by mouse input handling functions discussed later on.

The draw_slider function was added which handles drawing a single slider and it's control onto the screen based on the
current slider offset which is passed by the draw_slider_viewport function, discussed below.

draw_slider_viewport handles calling draw_slider 3 times with different translations to create an x, y and z slider
in the bottom right viewport, which it creates.
For each call to draw_slider, it passes a slider_offset, which is stored in 3 global variables,
slider1, 2 and 3 _offset, which are calculated and updated on the fly using the glut mouse motion/input callbacks.

The final top-level additions are the mouse_input and mouse_motion functions, which are registered as glut event
callbacks for when a mouse button is pressed or when the mouse moves, respectively.

mouse_input only responds to left mouse button events, and only those which fall
in the slider viewport coordinate range. When it detects the left mouse button is pressed down, it enables a global flag
called dragging, and conditionally enables a corresponding flag, dragging_x, y or z
based on the mouse's screen coordinates within the slider view. It then updates the slider offset and rotation angle
for the corresponding slider once, and initiates a redraw of the viewports.

mouse_event only response to mouse motion events so long as the dragging flag is set. Then, based on which
dragging_x, y or z flag was set, it again uses the mouses screen coordinates to update the slider offset and rotation
angle for the corresponding slider, and initiates a redraw of the viewports.