#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <OpenGL/gl.h>
#include <glut.h>
#else
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/glut.h>
#endif

#include "ply.h"
#include <cmath>
#include <cstdlib>

using namespace std;


int width = 1024;
int height = 768;

int num_faces;

Vertex **vlist;
Face **flist;

float min_x;
float max_x;

float min_y;
float max_y;

float min_z;
float max_z;

// The scale factors are calculated using the min and max x, y and z values
// to scale the model to fit in the viewport.
GLdouble x_scale_factor;
GLdouble y_scale_factor;
GLdouble z_scale_factor;

double slider1_offset = 0.0;
double slider2_offset = 0.0;
double slider3_offset = 0.0;

GLdouble x_rotation_angle = 0.0;
GLdouble y_rotation_angle = 0.0;
GLdouble z_rotation_angle = 0.0;

int dragging = 0;
int dragging_x = 0;
int dragging_y = 0;
int dragging_z = 0;


/**
 * Draw the rotation sliders into the bottom left viewport.
 */
void draw_slider(double slider_offset_x) {
    glColor3d(0.0, 0.0, 0.0);

    glBegin(GL_LINES);

    glVertex3d(-2.5, 0.5, 0);
    glVertex3d(2.5, 0.5, 0);

    glEnd();

    glBegin(GL_LINES);

    glVertex3d(2.5, 0.5, 0);
    glVertex3d(2.5, 0, 0);

    glEnd();

    glBegin(GL_LINES);

    glVertex3d(2.5, 0, 0);
    glVertex3d(-2.5, 0, 0);

    glEnd();

    glBegin(GL_LINES);

    glVertex3d(-2.5, 0, 0);
    glVertex3d(-2.5, 0.5, 0);

    glEnd();

    glColor3d(1.0, 0.0, 0.0);

    glPushMatrix();
    glTranslated(slider_offset_x, 0, 0);

    glBegin(GL_QUADS);

    glVertex3d(-2.5, 0.5, 0);
    glVertex3d(-2, 0.5, 0);
    glVertex3d(-2, 0, 0);
    glVertex3d(-2.5, 0, 0);

    glEnd();

    glPopMatrix();
}


/**
 * Render the loaded ply file onto the current viewport,
 * scaling and translating it to be centered at the origin and fit in our viewport.
 */
void render_ply() {
    glColor3d(0.65, 0.65, 0.65);

    glMatrixMode(GL_MODELVIEW);
    glRotated(x_rotation_angle, 1, 0, 0);
    glRotated(y_rotation_angle, 0, 1, 0);
    glRotated(z_rotation_angle, 0, 0, 1);
    glScaled(x_scale_factor, y_scale_factor, z_scale_factor);
    glTranslated(-(max_x + min_x) / 2.0, -(max_y + min_y) / 2.0, -(max_z + min_z) / 2.0);

    for (int i = 0; i < num_faces; i++) {
        Face* face = flist[i];
        int num_vertexes = face->nverts;

        GLenum render_shape;

        if (num_vertexes == 2) {
            render_shape = GL_LINE;
        }
        else if (num_vertexes == 3) {
            render_shape = GL_TRIANGLES;
        }
        else if (num_vertexes == 4) {
            render_shape = GL_QUADS;
        }
        else {
            render_shape = GL_POLYGON;
        }

        glBegin(render_shape);

        for (int j = 0; j < num_vertexes; j++) {
            int vertex_number = face->verts[j];
            Vertex* vertex_data = vlist[vertex_number];
            glVertex3f(vertex_data->x, vertex_data->y, vertex_data->z);
        }

        glEnd();

    }
}


/**
 * Draw x, y, and z axes centered around a vertex to the current viewport.
 */
void draw_axes() {
    // Z axis is blue.
    glColor3d(0.0, 0.0, 1.0);

    glBegin(GL_LINES);

    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 0 - 100);

    glEnd();

    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 0 + 100);

    glEnd();

    // Y axis is green.
    glColor3d(0.0, 1.0, 0.0);

    glBegin(GL_LINES);

    glVertex3f(0, 0, 0);
    glVertex3f(0, 0 + 100, 0);

    glEnd();

    glBegin(GL_LINES);

    glVertex3f(0, 0, 0);
    glVertex3f(0, 0 - 100, 0);

    glEnd();

    // X axis is red.
    glColor3d(1.0, 0.0, 0.0);

    glBegin(GL_LINES);

    glVertex3f(0, 0, 0);
    glVertex3f(0 + 100, 0, 0);

    glEnd();

    glBegin(GL_LINES);

    glVertex3f(0, 0, 0);
    glVertex3f(0 - 100, 0, 0);

    glEnd();
}


/**
 * Given the dimensions for a viewport, setup the necessary perspective projections,
 * camera and render the axes and ply file.
 * @param x The x origin for the viewport.
 * @param y The y origin for the viewport.
 * @param width The width of the viewport.
 * @param height The height of the viewport.
 */
void draw_viewport(int x, int y, int width, int height) {
    // The top-right viewport, top view of model.
    glViewport(x, y, width, height);

    // Setup the perspective and look down the z-axis.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(120.0, width / height, 0.1, 50.0);
    gluLookAt(0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Render the axes and ply file.
    draw_axes();
    render_ply();
}


/**
 * Define the slider viewport in the bottom right corner and draw all the sliders in their current positions.
 */
void draw_slider_viewport() {
    glViewport((width / 2) + 15, 0, (width / 2) - 30, height / 2);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-2.5, 2.5, -2.5, 2.5);

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();

    glTranslated(0.0, 2.0, 0.0);
    draw_slider(slider1_offset);

    glPopMatrix();
    glPushMatrix();

    draw_slider(slider2_offset);

    glPopMatrix();
    glPushMatrix();

    glTranslated(0.0, -2.0, 0.0);
    draw_slider(slider3_offset);

    glPopMatrix();
}


/**
 * OpenGL draw function, draw a top, side and front viewport of the ply file to the screen with axes.
 */
void draw() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the model 90 degrees around the x-axis to view the top.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(90, 1, 0, 0);

    // Top-left viewport, top view.
    draw_viewport(0, height / 2, width / 2, height / 2);

    // Rotate the model 90 degrees around the y-axis to view the side.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(90, 0, 1, 0);

    // Top-right viewport, side view.
    draw_viewport(width / 2, height / 2, width / 2, height / 2);

    // Bottom-left viewport, front view.
    glViewport(0, 0, width / 2, height / 2);

    // No rotation this time, front view.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    draw_viewport(0, 0, width / 2, height / 2);

    draw_slider_viewport();

    // Done rendering, swap the buffers.
    glutSwapBuffers();
}


/**
 * OpenGL resize function, update our width and height and redraw ply file.
 * @param new_width The new window width.
 * @param new_height The new window height.
 */
void resize(int new_width, int new_height) {
    // Update our width and height values, and redraw models.
    width = new_width;
    height = new_height;
    glutPostRedisplay();
}


/**
 * Code taken from the sample Visual Studio Solution posted on the class web page.
 * @param filename The name of the ply file to read in.
 * @post num_faces will be set to the number of faces defined in the ply file.
 *       vlist will be set to a list of Vertex pointers based on the vertices defined in the ply file.
 *       flist will be set to the list of Face pointers based on the faces defined in the ply file.
 *       min and max x, y and z values will be set based on the points in the vertices defined in the ply file.
 */
void read_ply_file(char* filename) {
    int i, j;
    PlyFile *ply;
    int nelems;
    char **elist;
    int file_type;
    float version;
    int nprops;
    char *elem_name;
    int num_elems;

    /* open a PLY file for reading */
    ply = ply_open_for_reading(filename, &nelems, &elist, &file_type, &version);

    /* print what we found out about the file */
    //printf("version %f\n", version);
    //printf("type %d\n", file_type);

    /* go through each kind of element that we learned is in the file */
    /* and read them */

    for (i = 0; i < nelems; i++) {

        /* get the description of the first element */
        elem_name = elist[i];
        // plist = ply_get_element_description(ply, elem_name, &num_elems, &nprops);
        ply_get_element_description(ply, elem_name, &num_elems, &nprops);

        /* if we're on vertex elements, read them in */
        if (equal_strings(const_cast<char*>("vertex"), elem_name)) {
            /* create a vertex list to hold all the vertices */
            vlist = (Vertex **)malloc(sizeof(Vertex *) * num_elems);

            /* set up for getting vertex elements */

            ply_get_property(ply, elem_name, &vert_props[0]);
            ply_get_property(ply, elem_name, &vert_props[1]);
            ply_get_property(ply, elem_name, &vert_props[2]);

            /* grab all the vertex elements */
            for (j = 0; j < num_elems; j++) {

                /* grab and element from the file */
                vlist[j] = (Vertex *)malloc(sizeof(Vertex));
                ply_get_element(ply, (void *)vlist[j]);

                if (j == 0) {
                    min_x = vlist[j]->x;
                    max_x = vlist[j]->x;
                    min_y = vlist[j]->y;
                    max_y = vlist[j]->y;
                    min_z = vlist[j]->z;
                    max_z = vlist[j]->z;
                }

                if (vlist[j]->x < min_x) {
                    min_x = vlist[j]->x;
                }
                else if (vlist[j]->x > max_x) {
                    max_x = vlist[j]->x;
                }

                if (vlist[j]->y < min_y) {
                    min_y = vlist[j]->y;
                }
                else if (vlist[j]->y > max_y) {
                    max_y = vlist[j]->y;
                }

                if (vlist[j]->z < min_z) {
                    min_z = vlist[j]->z;
                }
                else if (vlist[j]->z > max_z) {
                    max_z = vlist[j]->z;
                }
            }
        }

        if (equal_strings(const_cast<char*>("face"), elem_name)) {
            num_faces = num_elems;

            flist = (Face **)malloc(sizeof(Face *) * num_elems);

            ply_get_property(ply, elem_name, &face_props[0]);
            ply_get_property(ply, elem_name, &face_props[1]);

            for (j = 0; j < num_elems; j++) {
                flist[j] = (Face *)malloc(sizeof(Face));
                ply_get_element(ply, (void *)flist[j]);
            }
        }
    }

    ply_close(ply);

//    printf("X value ranges: %f - %f\n", min_x, max_x);
//    printf("Y value ranges: %f - %f\n", min_y, max_y);
//    printf("Z value ranges: %f - %f\n", min_z, max_z);
}


/**
 * Given screen coordinates x and y, calculate world coordinates relative to the given x and y.
 * @param x The screen x coordinate.
 * @param y The screen y coordinate.
 * @param world_x Pointer where the relative world x coordinate will be stored.
 * @param world_y Pointer where the relative world y coordinate will be stored.
 * @post world_x will contain the world relative x coordinate based on the screen x coordinate.
 *       world_y will contain the world relative y coordinate based on the screen y coordinate.
 */
void calculate_world_coordinates(int x, int y, double* world_x, double* world_y) {
    int viewport_left = width / 2;

    // The viewport x value is calculated by
    // subtracting the beginning x value of the viewport from the screen coordinate.
    double viewport_x = x - viewport_left;
    // The viewport y value is calculated by subtracting the screen coordinate from the height of the viewport,
    // as the screen coordinate y values are flipped relative to the world coordinate.
    double viewport_y = height - y;

    // The world x coordinate is calculated by using the range of the world x coordinates / the width of the viewport.
    *world_x = viewport_x * 5.0 / (width / 2);
    // The world y coordinate is calculated by using the range of the world y coordinates / the height of the viewport.
    *world_y = viewport_y * 2.5 / (height / 2);

    if (*world_x >= 4.75) {
        // 4.75 is the effective max x value after considering the padding of the sliders.
        *world_x = 4.75;
    } else if (*world_x <= .25) {
        // .25 is the effective minimum x value after considering the padding of the sliders.
        *world_x = .25;
    }
}


/**
 * Helper function to calculate the slider offset and rotation value given the world x coordinate of the slider.
 * @param world_x The x coordinate of the slider/mouse pointer.
 * @param slider_offset Pointer where the slider offset value should be stored.
 * @param rotation_angle Pointer where the slider rotation angle should be stored.
 * @post slider_offset will contain the slider offset relative to the world x coordinate.
 *       rotation_angle will contain the rotation angle relative to the world x coordinate.
 */
void calculate_slider_offsets(double world_x, double* slider_offset, GLdouble* rotation_angle) {
    // All we need to do for th slider offset based on the world x coordinate is remove the padding.
    *slider_offset = world_x - .25;
    // The rotation angle is defined as 360 = 4.5 x, so divide evenly based on that.
    *rotation_angle = (*slider_offset * 360) / 4.5;
}


/**
 * Callback function registered with GLUT to be called whenever a mouse button is pressed or released.
 * @param button The mouse button that pressed, one of GLUT_LEFT_BUTTON, GLUT_RIGHT_BUTTON, OR GLUT_MIDDLE_BUTTON.
 * @param state The state of the button, either pressed or released (GLUT_DOWN, GLUT_UP respectively).
 * @param x The screen x coordinate of the pointer when the button was pressed.
 * @param y The screen y coordinate of the pointer when the button was pressed.
 * @post If button is not GLUT_LEFT_BUTTON, no change.
 *       If the pointer was not in the slider viewport and state is GLUT_UP, dragging will be 0.
 *       dragging will be 1 if the button was pressed, 0 otherwise.
 *       One of dragging_x, dragging_y, or dragging_z will be set to 1, based on matching the screen coordinate to
 *          one of the sliders in the viewport, the others will be 0.
 *       Based on the location of the pointer:
 *          slider1_offset, slider2_offset or slider3_offset will be set to the offset of the pointer within the slider.
 *          x_rotation_angle, y_rotation_angle or z_rotation_angle will be set to the rotation angle based on
 *              the relevant slider offset.
 *       The draw function will be called at the end of the glut event loop.
 */
void mouse_input(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON) {
        // We're only interested in left mouse button presses.
        return;
    } else if (!(x >= (width / 2) && y >= (height / 2))) {
        // We're not in the bottom-right viewport,
        // so the only thing that could be necessary is updating the dragging state.
        if (state != GLUT_UP) {
            // The button was pressed, but we're not within the viewport so nothing to do.
            // However, if we are already dragging, and the button was released,
            // even though the pointer isn't within the viewport, we still want to stop dragging.
            return;
        }
    }

    if (state == GLUT_UP) {
        // The button was released so stop dragging now.
        // We don't return here, because we still want to ensure
        // the offsets and angles get updated properly one last time.
        dragging = 0;
    }

    double world_x;
    double world_y;

    dragging_x = 0;
    dragging_y = 0;
    dragging_z = 0;

    dragging = 1;

    calculate_world_coordinates(x, y, &world_x, &world_y);

    if (world_y >= 2.0 && world_y <= 2.5) {
        // The X slider is selected
        calculate_slider_offsets(world_x, &slider1_offset, &x_rotation_angle);
        dragging_x = 1;
    } else if (world_y >= 1.0 && world_y <= 1.5) {
        // The Y slider is selected
        calculate_slider_offsets(world_x, &slider2_offset, &y_rotation_angle);
        dragging_y = 1;
    } else if (world_y >= 0 && world_y <= 0.5) {
        // The Z slider is selected
        calculate_slider_offsets(world_x, &slider3_offset, &z_rotation_angle);
        dragging_z = 1;
    }

    glutPostRedisplay();
}


void mouse_motion(int x, int y) {
    if (dragging == 0) {
        return;
    }

    double world_x;
    double world_y;

    calculate_world_coordinates(x, y, &world_x, &world_y);

    if (dragging_x) {
        calculate_slider_offsets(world_x, &slider1_offset, &x_rotation_angle);
    } else if (dragging_y) {
        calculate_slider_offsets(world_x, &slider2_offset, &y_rotation_angle);
    } else if (dragging_z) {
        calculate_slider_offsets(world_x, &slider3_offset, &z_rotation_angle);
    }

    glutPostRedisplay();
}


/**
 * Application entry point, parse command-line, load the ply file, setup the glut window, and enter event loop.
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments.
 * @return Return code.
 */
int main(int argc, char** argv) {
    char* filename;

    if (argc < 2) {
        printf("You must provide the path to the ply file to render!");
        return 100;
    }
    else {
        filename = argv[1];
    }

    read_ply_file(filename);

    float x_range = abs(max_x - min_x);
    float y_range = abs(max_y - min_y);
    float z_range = abs(max_z - min_z);

    x_scale_factor = 2.0 / x_range;
    y_scale_factor = 2.0 / y_range;
    z_scale_factor = 2.0 / z_range;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(width, height);
    glutCreateWindow("3D PLY Model Viewer");
    glutDisplayFunc(draw);
    glutReshapeFunc(resize);
    glutMouseFunc(mouse_input);
    glutMotionFunc(mouse_motion);

    glEnable(GL_DEPTH_TEST);

    glutMainLoop();

    return 0;
}
