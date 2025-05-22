#include <iostream>
#include <fstream>
#include "imageloader.h"

using namespace std;

// Constructor for an image from pixel data
Image::Image(unsigned char* ps, int w, int h) {
    width = w;
    height = h;
    pixels = ps;
}

// Constructor for creating a default texture (for error cases)
Image::Image(int w, int h) {
    width = w;
    height = h;
    pixels = new unsigned char[w * h * 3];
    
    // Create a simple checkerboard pattern
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int index = (y * w + x) * 3;
            if ((x + y) % 2 == 0) {
                // Red square
                pixels[index] = 255;     // R
                pixels[index + 1] = 0;   // G
                pixels[index + 2] = 0;   // B
            } else {
                // White square
                pixels[index] = 255;     // R
                pixels[index + 1] = 255; // G
                pixels[index + 2] = 255; // B
            }
        }
    }
}

Image::~Image() {
    // Only delete if pixels is not nullptr
    if (pixels != nullptr) {
        delete[] pixels;
        pixels = nullptr;
    }
}

int toInt(const char* bytes, int offset) {
    return (int)(unsigned char)bytes[offset] |
           ((int)(unsigned char)bytes[offset + 1] << 8) |
           ((int)(unsigned char)bytes[offset + 2] << 16) |
           ((int)(unsigned char)bytes[offset + 3] << 24);
}

Image* loadBMP(const char* filename) {
    ifstream input;
    input.open(filename, ifstream::binary);
    if (input.fail()) {
        cerr << "Could not find file: " << filename << endl;
        return nullptr;
    }
    
    cout << "File opened successfully: " << filename << endl;
    
    char header[54];
    input.read(header, 54);
    
    if (input.gcount() != 54) {
        cerr << "Invalid BMP file (header too short): " << filename << endl;
        input.close();
        return nullptr;
    }
    
    // Check BMP signature (first two bytes should be 'B' and 'M')
    if (header[0] != 'B' || header[1] != 'M') {
        cerr << "Invalid BMP signature in file: " << filename << endl;
        input.close();
        return nullptr;
    }

    int width = toInt(header, 18);
    int height = toInt(header, 22);
    int dataOffset = toInt(header, 10);
    int imageSize = toInt(header, 34);
    int bitsPerPixel = (unsigned char)header[28] | ((unsigned char)header[29] << 8);
    
    cout << "BMP info: width=" << width << ", height=" << height
         << ", bitsPerPixel=" << bitsPerPixel
         << ", dataOffset=" << dataOffset << ", imageSize=" << imageSize << endl;
    
    // If image size is not specified or incorrect, calculate it
    if (imageSize == 0 || imageSize > width * height * 4) {
        // Default to 24 bits per pixel (3 bytes) if reasonable
        imageSize = width * height * 3;
        cout << "Image size not specified or incorrect, calculated as: " << imageSize << endl;
    }
    
    if (width <= 0 || height <= 0 || imageSize <= 0) {
        cerr << "Invalid BMP dimensions: " << filename << endl;
        input.close();
        return nullptr;
    }
    
    // Check if we need to handle row padding (BMP rows are padded to 4-byte boundaries)
    int rowSize = width * 3;  // 3 bytes per pixel (RGB)
    int paddedRowSize = (rowSize + 3) & (~3);  // Pad to 4-byte boundary
    bool needsPadding = (rowSize != paddedRowSize);
    
    if (needsPadding) {
        cout << "BMP has row padding: actual row size = " << rowSize
             << ", padded = " << paddedRowSize << endl;
    }
    
    // Move to the pixel data
    input.seekg(dataOffset, ios::beg);
    
    // Read the image data, accounting for potential padding
    unsigned char* data = new unsigned char[imageSize];
    if (needsPadding) {
        // Handle padding manually
        unsigned char* rowBuffer = new unsigned char[paddedRowSize];
        for (int y = 0; y < height; y++) {
            input.read(reinterpret_cast<char*>(rowBuffer), paddedRowSize);
            // Copy just the pixel data without padding
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < 3; c++) {
                    data[(height - 1 - y) * width * 3 + x * 3 + c] = rowBuffer[x * 3 + c];
                }
            }
        }
        delete[] rowBuffer;
    } else {
        // No padding, can read all at once
        input.read(reinterpret_cast<char*>(data), imageSize);
    }
    
    if (input.fail() && !input.eof()) {
        cerr << "Error reading image data from " << filename << endl;
        delete[] data;
        input.close();
        return nullptr;
    }
    
    input.close();
    
    // Convert BGR (BMP format) to RGB (OpenGL format)
    unsigned char* pixels = new unsigned char[width * height * 3];
    for (int i = 0; i < width * height; i++) {
        pixels[i * 3] = data[i * 3 + 2];       // R (from B)
        pixels[i * 3 + 1] = data[i * 3 + 1];   // G
        pixels[i * 3 + 2] = data[i * 3];       // B (from R)
    }
    
    delete[] data;
    
    cout << "Successfully loaded BMP: " << filename << endl;
    return new Image(pixels, width, height);
}
