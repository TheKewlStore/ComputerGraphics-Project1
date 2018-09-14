#if defined(__APPLE__)
    #include <OpenGL/gl.h>
    #include <glut.h>
#else
#include <GL/gl.h>
#include <Gl/glut.h>
#endif

#include "plyfile.cpp"
#include "cmd_options.h"

using namespace std;
#include <iostream>


char* VERTEX_PROPERTY_NAME = (char*) "vertex";
char* FACE_PROPERTY_NAME = (char*) "face";

int num_vertex_elements = 0;
int num_vertex_props = 0;
int num_face_elements = 0;
int num_face_props = 0;

PlyProperty** vertexProperties = nullptr;
PlyProperty** faceProperties = nullptr;
PlyElement** vertexElements = nullptr;
PlyElement** faceElements = nullptr;


void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
}


void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_POLYGON);

    glEnd();
    glFlush();
}


void reshape(GLsizei width, GLsizei height) {
}


void loadPlyFile(std::string &filename) {
    PlyFile* ply_file;
    int num_elements = 0;
    char** element_names = nullptr;
    int file_type = 0;
    float version = 0;
    auto *y = new char[filename.length() + 1];

    std::strcpy(y, filename.c_str());
    ply_file = ply_open_for_reading(y, &num_elements, &element_names, &file_type, &version);
    delete[] y;

    vertexProperties = ply_get_element_description(ply_file, VERTEX_PROPERTY_NAME, &num_vertex_elements, &num_vertex_props);
    faceProperties = ply_get_element_description(ply_file, FACE_PROPERTY_NAME, &num_face_elements, &num_face_props);

    for (int i=0; i < num_vertex_props; i++) {
        PlyProperty* prop = vertexProperties[i];
        ply_get_property(ply_file, VERTEX_PROPERTY_NAME, prop);
    }

    vertexElements = new PlyElement*[num_vertex_elements];
    faceElements = new PlyElement*[num_face_elements];

    for (int i=0; i < num_vertex_elements; i++) {
        ply_get_element(ply_file, vertexElements[i]);
    }

    for (int i=0; i < num_face_props; i++) {
        PlyProperty* prop = faceProperties[i];
        ply_get_property(ply_file, FACE_PROPERTY_NAME, prop);
    }

    for (int i=0; i < num_face_elements; i++) {
        PlyElement* element;
        ply_get_element(ply_file, &element);
        faceElements[i] = element;
    }
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(50, 100);
    glutInitWindowSize(400, 300);
    glutCreateWindow("3D Model Viewer");
    init();
    glutDisplayFunc(draw);
    glutReshapeFunc(reshape);

    InputParser input(argc, argv);
    std::string filename;

    if (input.cmdOptionExists("-f")) {
        filename = input.getCmdOption("-f");
    } else if (input.cmdOptionExists("--filename")) {
        filename = input.getCmdOption("--filename");
    } else {
        cout << "Missing required filename argument!";
        return -1;
    }

    loadPlyFile(filename);

    glutMainLoop();
    return 0;
}
