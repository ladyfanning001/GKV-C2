#include <iostream>
#include <stdlib.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h> // For getcwd() on Unix/Linux/macOS
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "imageloader.h"

using namespace std;

const float BOX_SIZE = 7.0f;
float _angle = 0;

GLuint _textureId, _textureId1, _textureId2, _textureId3, _textureId4, _textureId5;

// Function to print current working directory (for debugging)
void printWorkingDirectory() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, buffer);
    std::cout << "Current working directory: " << buffer << std::endl;
#else
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::cout << "Current working directory: " << cwd << std::endl;
    } else {
        std::cerr << "getcwd() error" << std::endl;
    }
#endif
}

bool loadImageWithDebug(const char* filename, Image** imagePtr, const char* textureLabel) {
    std::cout << "Attempting to load " << textureLabel << ": " << filename << std::endl;
    
    // Define paths to try
    std::string paths[] = {
        filename,
        "./" + std::string(filename),
        "../" + std::string(filename),
        "../../" + std::string(filename),
        "../Resources/" + std::string(filename)
    };
    
    for (int i = 0; i < 5; i++) {
        std::cout << "Trying path: " << paths[i] << std::endl;
        *imagePtr = loadBMP(paths[i].c_str());
        if (*imagePtr) {
            std::cout << "Successfully loaded " << textureLabel << " from: " << paths[i] << std::endl;
            return true;
        }
    }
    
    std::cerr << "Failed to load " << textureLabel << " from any path" << std::endl;
    return false;
}

GLuint loadTexture(Image* image) {
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    
    return textureId;
}

void initRendering() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
    
    // Print working directory for debugging
    printWorkingDirectory();
    
    // Load each texture individually with better error handling
    Image* image = nullptr;
    if (loadImageWithDebug("bg1.bmp", &image, "Texture 0")) {
        _textureId = loadTexture(image);
        delete image;
    } else {
        std::cerr << "Using default texture for texture 0" << std::endl;
        // Create a default 2x2 checkerboard texture
        unsigned char* pixels = new unsigned char[12];
        pixels[0] = 255; pixels[1] = 0;   pixels[2] = 0;
        pixels[3] = 0;   pixels[4] = 255; pixels[5] = 0;
        pixels[6] = 0;   pixels[7] = 0;   pixels[8] = 255;
        pixels[9] = 255; pixels[10] = 255; pixels[11] = 0;
        
        Image* defaultImage = new Image(pixels, 2, 2);
        _textureId = loadTexture(defaultImage);
        delete defaultImage; // The Image destructor will delete the pixels
    }
    
    // Repeat for other textures
    if (loadImageWithDebug("bg1.bmp", &image, "Texture 1")) {
        _textureId1 = loadTexture(image);
        delete image;
    } else {
        _textureId1 = _textureId; // Reuse the first texture
    }
    
    if (loadImageWithDebug("bg1.bmp", &image, "Texture 2")) {
        _textureId2 = loadTexture(image);
        delete image;
    } else {
        _textureId2 = _textureId; // Reuse the first texture
    }
    
    if (loadImageWithDebug("bg1.bmp", &image, "Texture 3")) {
        _textureId3 = loadTexture(image);
        delete image;
    } else {
        _textureId3 = _textureId; // Reuse the first texture
    }
    
    if (loadImageWithDebug("bg1.bmp", &image, "Texture 4")) {
        _textureId4 = loadTexture(image);
        delete image;
    } else {
        _textureId4 = _textureId; // Reuse the first texture
    }
    
    if (loadImageWithDebug("bg1.bmp", &image, "Texture 5")) {
        _textureId5 = loadTexture(image);
        delete image;
    } else {
        _textureId5 = _textureId; // Reuse the first texture
    }
    
    std::cout << "All textures initialized successfully" << std::endl;
}

void handleKeypress(unsigned char key, int x, int y) {
    if (key == 27) exit(0); // ESC key
}

void handleResize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / h, 1.0, 200.0);
}

void drawFace(GLuint texture, float nx, float ny, float nz,
              float v0[3], float v1[3], float v2[3], float v3[3]) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glNormal3f(nx, ny, nz);
    glTexCoord2f(0.0f, 0.0f); glVertex3fv(v0);
    glTexCoord2f(1.0f, 0.0f); glVertex3fv(v1);
    glTexCoord2f(1.0f, 1.0f); glVertex3fv(v2);
    glTexCoord2f(0.0f, 1.0f); glVertex3fv(v3);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -20.0f);

    GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    GLfloat lightColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat lightPos[] = {-2 * BOX_SIZE, BOX_SIZE, 4 * BOX_SIZE, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glRotatef(-_angle, 1.0f, 1.0f, 0.0f);

    float hs = BOX_SIZE / 2;

    float top[4][3]    = {{-hs, hs, -hs}, {-hs, hs, hs}, {hs, hs, hs}, {hs, hs, -hs}};
    float bottom[4][3] = {{-hs, -hs, -hs}, {hs, -hs, -hs}, {hs, -hs, hs}, {-hs, -hs, hs}};
    float left[4][3]   = {{-hs, -hs, -hs}, {-hs, -hs, hs}, {-hs, hs, hs}, {-hs, hs, -hs}};
    float right[4][3]  = {{hs, -hs, -hs}, {hs, hs, -hs}, {hs, hs, hs}, {hs, -hs, hs}};
    float front[4][3]  = {{-hs, -hs, hs}, {hs, -hs, hs}, {hs, hs, hs}, {-hs, hs, hs}};
    float back[4][3]   = {{-hs, -hs, -hs}, {-hs, hs, -hs}, {hs, hs, -hs}, {hs, -hs, -hs}};

    drawFace(_textureId,   0,  1,  0, top[0], top[1], top[2], top[3]);
    drawFace(_textureId1,  0, -1,  0, bottom[0], bottom[1], bottom[2], bottom[3]);
    drawFace(_textureId2, -1,  0,  0, left[0], left[1], left[2], left[3]);
    drawFace(_textureId3,  1,  0,  0, right[0], right[1], right[2], right[3]);
    drawFace(_textureId4,  0,  0,  1, front[0], front[1], front[2], front[3]);
    drawFace(_textureId5,  0,  0, -1, back[0], back[1], back[2], back[3]);

    glutSwapBuffers();
}

void update(int value) {
    _angle += 1.0f;
    if (_angle > 360) _angle -= 360;
    glutPostRedisplay();
    glutTimerFunc(20, update, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400, 400);
    glutCreateWindow("Textured Cube in Xcode");
    initRendering();
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(25, update, 0);
    glutMainLoop();
    return 0;
}
