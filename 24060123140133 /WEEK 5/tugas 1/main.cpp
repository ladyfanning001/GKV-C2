#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

double rotX = 10, rotY = 10;
double speedMultiplier = 1.0;

void init()
{
    glClearColor(0, 0, 0, 0);
    glClearDepth(1.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

    glRotated(rotX, 1, 0, 0);
    glRotated(rotY, 0, 1, 0);

    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.5, 0.8, 1.0);
    glutSolidTeapot(1);

    glPopMatrix();
    glFlush();
    glutSwapBuffers();
}

void specialKeys(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_LEFT:  rotY -= 5 * speedMultiplier; break;
        case GLUT_KEY_RIGHT: rotY += 5 * speedMultiplier; break;
        case GLUT_KEY_UP:    rotX -= 5 * speedMultiplier; break;
        case GLUT_KEY_DOWN:  rotX += 5 * speedMultiplier; break;
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'q': exit(0);
        case '+': speedMultiplier += 0.2; break;
        case '-': if (speedMultiplier > 0.2) speedMultiplier -= 0.2; break;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutCreateWindow("Rotating Teapot - macOS Version");

    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);
    init();
    glutMainLoop();

    return 0;
}
