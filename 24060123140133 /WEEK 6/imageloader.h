#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

class Image {
public:
    int width, height;
    unsigned char* pixels;
    
    // Constructor for creating a new image from pixel data
    Image(unsigned char* pixels, int width, int height);
    
    // Constructor for creating a default texture (for error cases)
    Image(int width, int height);
    
    // Destructor
    ~Image();
    
private:
    // Prevent copying to avoid double-free issues
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
};

// Load a BMP file and return an Image object
Image* loadBMP(const char* filename);

// Helper function to convert bytes to int (for BMP header parsing)
int toInt(const char* bytes, int offset);

// Function to create an OpenGL texture from an image
GLuint loadTexture(Image* image);

#endif // IMAGELOADER_H
