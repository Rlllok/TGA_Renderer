#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "tgaimage.h"

// IMAGE
Image::Image()
    : data(NULL), width(0), height(0), bpp(0) {}


Image::Image(unsigned int w, unsigned int h, unsigned int bpp) : width(w), height(h),  bpp(bpp) {
    unsigned long nbytes = w * h * bpp;
    data = new unsigned char[nbytes];
    memset(data, 0, nbytes);
}


Image::~Image() {
    if (data) delete [] data;
}


bool Image::write_file(const char* filename) {
    ofstream output;

    output.open(filename, ios::binary);

    if (!output.is_open()) {
        cerr << "can't open the file." << endl;
        output.close();
        return false;
    }

    Header header = {};
    memset((void *)&header, 0, sizeof(header));
    header.img_bitspp = bpp<<3; // multiply bpp by 8
    header.img_width = width;
    header.img_height = height;
    header.img_type = (bpp==GRAYSCALE_BPP?3:2);
    header.img_descriptor = 0x20;
    output.write((char*)&header, sizeof(header));

    unsigned long nbytes = width * height * bpp;
    output.write((char*)data, nbytes);

    output.close();

    return true;
}


bool Image::read_file(const char *filename) {
    if (data) delete [] data;
    ifstream input_file;
    input_file.open(filename, ios::binary);

    if (!input_file.is_open()) {
        cerr << "Cannot open file." << endl;
        input_file.close();
        return false;
    }

    Header header;
    input_file.read((char*)&header, sizeof(header));
    if (!input_file.good()) {
        cerr << "Cannot read the header." << endl;
        input_file.close();
        return false;
    }

    width = header.img_width;
    height = header.img_height;
    bpp = header.img_bitspp>>3;
    if (width<=0 || height <=0 || (bpp!=GRAYSCALE_BPP && bpp!=RGB_BPP && bpp!=RGBA_BPP)) {
        cerr << "Wrong height, width or bytes per pixel." << endl;
        input_file.close();
        return false;
    }

    unsigned long nbytes = bpp*width*height;
    data = new unsigned char[nbytes];
    if (header.img_type==3 || header.img_type==2) {
        input_file.read((char*)data, nbytes);
        if (!input_file.good()) {
            cerr << "Cannot read the data." << endl;
            input_file.close();
            return false;
        }
    } else {
        cerr << "Unknown data format." << endl;
        input_file.close();
        return false;
    }

    return true;
}


bool Image::setPixel(int x, int y, Pixel p) {
    if (!data || x < 0 || y < 0 || x >= width || y >= height) {
        cout << x << " w: " << width << endl;
        cout << y << " h: " << height << endl;
        cerr << "Cannot set pixel value." << endl;
        throw invalid_argument("Wrong argument.");
        return false;
    }
    if (p.get_bpp() != bpp) {
        throw invalid_argument("Wrong Pixel Type.");
    }
    memcpy(data+(x+y*width)*bpp, p.get_bytes(), bpp);
    return true;
}


void Image::drawLine(int x0, int y0, int x1, int y1, Pixel pixel) {
    bool steep = false;
    if (abs(x1 - x0) < abs(y1 - y0)) {
        swap(x0, y0);
        swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = abs(y1 - y0);
    int y_step = (y0<y1 ? 1: -1);
    int error = 0;
    int y = y0;
    
    for (int x = x0; x <= x1; x++) {
        if (steep)
            this->setPixel(y, x, pixel);
        else
            this->setPixel(x, y, pixel);
        error += dy;
        if ((error << 1) >= dx) {
            y += y_step;
            error -= dx;
        }
    }
}

// for testing
void Image::random_data() {
    srand(time(NULL));
    unsigned long nbytes = width * height * bpp;
    for (int i = 0; i < nbytes; i++) {
        data[i] = rand();
    }
}

// PIXEL 
Pixel::Pixel() : bpp(0) {}


Pixel::Pixel(unsigned char v) : bpp(1) {
    bytes[0] = v;
}


Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b) : bpp(3) {
    bytes[0] = b;
    bytes[1] = g;
    bytes[2] = r;
}


Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : bpp(4) {
    bytes[0] = b;
    bytes[1] = g;
    bytes[2] = r;
    bytes[3] = a;
}


int Pixel::get_bpp() {
    return this->bpp;
}

unsigned char* Pixel::get_bytes() {
    return this->bytes;
}