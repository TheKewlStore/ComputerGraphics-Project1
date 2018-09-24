#ifdef WIN32
#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/glut.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <glut.h>
#endif

#include <cmath>

#include "ply.h"
#include "plyfile.cpp"

using namespace std;
#include <iostream>


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


/**
 * Render the loaded ply file onto the current viewport,
 * scaling and translating it to be centered at the origin and fit in our viewport.
 */
void renderPLY() {
    glColor3d(0.65, 0.65, 0.65);

    glMatrixMode(GL_MODELVIEW);
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
 * @param center_vertex The vertex to center the axes around.
 */
void drawAxes(Vertex center_vertex) {
    // Z axis is blue.
    glColor3d(0.0, 0.0, 1.0);

    glBegin(GL_LINES);

    glVertex3f(center_vertex.x, center_vertex.y, center_vertex.z);
    glVertex3f(center_vertex.x, center_vertex.y, center_vertex.z - 100);

    glEnd();

    glVertex3f(center_vertex.x, center_vertex.y, center_vertex.z);
    glVertex3f(center_vertex.x, center_vertex.y, center_vertex.z + 100);

    glEnd();

    // Y axis is green.
    glColor3d(0.0, 1.0, 0.0);

    glBegin(GL_LINES);

    glVertex3f(center_vertex.x, center_vertex.y, center_vertex.z);
    glVertex3f(center_vertex.x, center_vertex.y + 100, center_vertex.z);

    glEnd();

    glBegin(GL_LINES);

    glVertex3f(center_vertex.x, center_vertex.y, center_vertex.z);
    glVertex3f(center_vertex.x, center_vertex.y - 100, center_vertex.z);

    glEnd();

    // X axis is red.
    glColor3d(1.0, 0.0, 0.0);

    glBegin(GL_LINES);

    glVertex3f(center_vertex.x, center_vertex.y, center_vertex.z);
    glVertex3f(center_vertex.x + 100, center_vertex.y, center_vertex.z);

    glEnd();

    glBegin(GL_LINES);

    glVertex3f(center_vertex.x, center_vertex.y, center_vertex.z);
    glVertex3f(center_vertex.x - 100, center_vertex.y, center_vertex.z);

    glEnd();
}


/**
 * OpenGL draw function, draw a top, side and front viewport of the ply file to the screen with axes.
 */
void draw() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // The center_vertex is also 0, 0, 0 now in favor of just translating the model to the origin.
    Vertex center_vertex = Vertex();

    center_vertex.x = 0.0;
    center_vertex.y = 0.0;
    center_vertex.z = 0.0;

    // The top-right viewport, top view of model.
    glViewport(0, height / 2, width / 2, height / 2);

    // Rotate the model 90 degrees around the x-axis to view the top.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(90, 1, 0, 0);

    // Setup the perspective and look down the z-axis.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(120.0, width / height, 0.1, 50.0);
    gluLookAt(0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Render the axes and ply file.
    drawAxes(center_vertex);
    renderPLY();

    // Top-right viewport, side view of model.
    glViewport(width / 2, height / 2, width / 2, height / 2);

    // Rotate the model 90 degrees around the y-axis to view the side.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(90, 0, 1, 0);

    // Setup the perspective and look down the z-axis.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(120.0, width / height, 0.1, 50.0);
    gluLookAt(0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Render the axes and ply file.
    drawAxes(center_vertex);
    renderPLY();

    // Bottom-left viewport, front view.
    glViewport(0, 0, width / 2, height / 2);

    // No rotation this time, front view.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Setup the perspective and look down the z-axis.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(120.0, width / height, 0.1, 50.0);
    gluLookAt(0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Render the axes and ply file.
    drawAxes(center_vertex);
    renderPLY();

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
    int i, j, k;
    PlyFile *ply;
    int nelems;
    char **elist;
    int file_type;
    float version;
    int nprops;
    PlyProperty **plist;
    char *elem_name;
    int num_comments;
    char **comments;
    int num_obj_info;
    char **obj_info;
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
        plist = ply_get_element_description(ply, elem_name, &num_elems, &nprops);

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
        exit(100);
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

    glEnable(GL_DEPTH_TEST);

    glutMainLoop();

    return 0;
}