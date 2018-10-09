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

FILE* fp;

int num_faces;
int num_vertices;
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
void drawSlider(double slider_offset_x) {
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
void renderPLY() {
    glColor3d(0.65, 0.65, 0.65);

    glMatrixMode(GL_MODELVIEW);
    glScaled(x_scale_factor, y_scale_factor, z_scale_factor);
    glTranslated(-(max_x + min_x) / 2.0, -(max_y + min_y) / 2.0, -(max_z + min_z) / 2.0);
    glRotated(x_rotation_angle, 1, 0, 0);
    glRotated(y_rotation_angle, 0, 1, 0);
    glRotated(z_rotation_angle, 0, 0, 1);

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
void drawAxes() {
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


void drawViewport(int x, int y, int width, int height) {
    // The top-right viewport, top view of model.
    glViewport(x, y, width, height);

    // Setup the perspective and look down the z-axis.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(120.0, width / height, 0.1, 50.0);
    gluLookAt(0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Render the axes and ply file.
    drawAxes();
    renderPLY();
}


void drawSliderViewport() {
    glViewport((width / 2) + 15, 0, (width / 2) - 30, height / 2);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-2.5, 2.5, -2.5, 2.5);

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();

    glTranslated(0.0, 2.0, 0.0);
    drawSlider(slider1_offset);

    glPopMatrix();
    glPushMatrix();

    drawSlider(slider2_offset);

    glPopMatrix();
    glPushMatrix();

    glTranslated(0.0, -2.0, 0.0);
    drawSlider(slider3_offset);

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
    drawViewport(0, height / 2, width / 2, height / 2);

    // Rotate the model 90 degrees around the y-axis to view the side.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(90, 0, 1, 0);

    // Top-right viewport, side view.
    drawViewport(width / 2, height / 2, width / 2, height / 2);

    // Bottom-left viewport, front view.
    glViewport(0, 0, width / 2, height / 2);

    // No rotation this time, front view.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawViewport(0, 0, width / 2, height / 2);

    drawSliderViewport();

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
        if (equal_strings("vertex", elem_name)) {
            num_vertices = num_elems;

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

        if (equal_strings("face", elem_name)) {
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

    printf("X value ranges: %f - %f\n", min_x, max_x);
    printf("Y value ranges: %f - %f\n", min_y, max_y);
    printf("Z value ranges: %f - %f\n", min_z, max_z);
}


void calculateSliderOffset(int x, int y, double* world_x, double* world_y) {
    int viewport_left = width / 2;

    double viewport_x = x - viewport_left;
    double viewport_y = height - y;

    *world_x = viewport_x * 5.0 / (width / 2);
    *world_y = viewport_y * 2.5 / (height / 2);

    if (*world_x >= 4.75) {
        *world_x = 4.75;
    } else if (*world_x <= .25) {
        *world_x = .25;
    }
}


void mouse_input(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON) {
        return;
    } else if (!(x >= (width / 2) && y >= (height / 2))) {
        // We're not in the bottom-right viewport.
        return;
    }

    if (state == GLUT_UP) {
        dragging = 0;
    }

    double world_x;
    double world_y;

    dragging_x = 0;
    dragging_y = 0;
    dragging_z = 0;

    dragging = 1;

    calculateSliderOffset(x, y, &world_x, &world_y);

    if (world_y >= 2.0 && world_y <= 2.5) {
        // The X slider is selected
        slider1_offset = world_x - .25;
        x_rotation_angle = (slider1_offset * 360) / 4.5;
        dragging_x = 1;
    } else if (world_y >= 1.0 && world_y <= 1.5) {
        // The Y slider is selected
        slider2_offset = world_x - .25;
        y_rotation_angle = (slider2_offset * 360) / 4.5;
        dragging_y = 1;
    } else if (world_y >= 0 && world_y <= 0.5) {
        // The Z slider is selected
        slider3_offset = world_x - .25;
        z_rotation_angle = (slider3_offset * 360) / 4.5;
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

    calculateSliderOffset(x, y, &world_x, &world_y);

    if (dragging_x) {
        slider1_offset = world_x - .25;
        x_rotation_angle = (slider1_offset * 360) / 4.5;
        printf("%f\n", slider1_offset);
    } else if (dragging_y) {
        slider2_offset = world_x - .25;
        y_rotation_angle = (slider2_offset * 360) / 4.5;
    } else if (dragging_z) {
        slider3_offset = world_x - .25;
        z_rotation_angle = (slider3_offset * 360) / 4.5;
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
