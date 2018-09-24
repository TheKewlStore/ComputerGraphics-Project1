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

int number_elements = 0;
char*** element_names = (char***) malloc (sizeof(char*));
FILE* fp;

int width = 1024;
int height = 768;

int num_faces;
float** allVertices;
int** allFaces;
//auto start = std::chrono::system_clock::now();


void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
}


void readVertices(PlyElement* element) {
    allVertices = new float*[element->num];

    int num_words;
    char* original_line;

    for (int line = 0; line < element->num; line++) {
        float* vertices = new float[3];
        char** element_data = get_words(fp, &num_words, &original_line);

        if (num_words < 3) {
            printf("Not enough arguments for vertex number: " + line + 1);
            exit(100);
        } else {
            vertices[0] = (float) strtod(element_data[0], nullptr);
            vertices[1] = (float) strtod(element_data[1], nullptr);
            vertices[2] = (float) strtod(element_data[2], nullptr);
            allVertices[line] = vertices;
        }
    }
}


void readFaces(PlyElement* element) {
    num_faces = element->num;
    allFaces = new int*[element->num];

    int num_words;
    char* original_line;

    for (int line=0; line < element->num; line++) {
        char** element_data = get_words(fp, &num_words, &original_line);
        int* vertex_numbers = new int[num_words];

        for (int i=0; i < num_words; i++) {
            vertex_numbers[i] = atoi(element_data[i]);
        }

        allFaces[line] = vertex_numbers;
    }
}


void renderPLY() {
    glColor3d(1.0, 0.0, 1.0);

    for (int i=0; i < num_faces; i++) {
        int* vertex_numbers = allFaces[i];
        int num_vertexes = vertex_numbers[0];

        GLenum render_shape;

        if (num_vertexes == 2) {
            render_shape = GL_LINE;
        } else if (num_vertexes == 3) {
            render_shape = GL_TRIANGLES;
        } else if (num_vertexes == 4) {
            render_shape = GL_QUADS;
        } else {
            render_shape = GL_POLYGON;
        }

        glBegin(render_shape);

        for (int j=1; j < num_vertexes; j++) {
            int vertex_number = vertex_numbers[j];
            float* vertex_data = allVertices[vertex_number];
            glVertex3fv(vertex_data);
        }

        glEnd();
    }
}


void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glRotatef(1.0, 1.0, 0.0, 0.0);

    glViewport(0, height / 2, (width / 2), height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, ((width / 2) / height / 2), 0.1, 50.0);
    glTranslatef(0.0, 0.0, -10);
    gluLookAt(0.0, 1.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    renderPLY();

    glViewport((width / 2), (height / 2), width, height);
    glMatrixMode(GL_PROJECTION);
    gluLookAt(1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    renderPLY();

    glViewport(0, 0, width / 2, height / 2);
    glMatrixMode(GL_PROJECTION);
    gluLookAt(0.0, 2.0, 1.0, 0.0, 0.5, 0.0, 0.0, 1.0, 0.0);
    renderPLY();

    glutSwapBuffers();
    glutPostRedisplay();
}


//void reshape(GLsizei width, GLsizei height) {
//    glViewport(0, 0, width, height);
//}


void loadPlyFile(std::string &filename) {
    fp = fopen(filename.c_str(), "r");
    PlyFile* ply_file = ply_read(fp, &number_elements, element_names);

    for (int i=0; i < ply_file->nelems; i++) {
        PlyElement* element = ply_file->elems[i];
        if (strcmp(element->name, VERTEX_PROPERTY_NAME) == 0) {
            readVertices(element);
        } else if (strcmp(element->name, FACE_PROPERTY_NAME) == 0) {
            readFaces(element);
        }
    }
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowPosition(50, 100);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("3D Model Viewer");
    init();
    glutDisplayFunc(draw);
//    glutReshapeFunc(reshape);

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
