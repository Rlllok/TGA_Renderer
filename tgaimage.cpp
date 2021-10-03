#include <fstream>
#include <iostream>
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


Image::~Image() {}


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
    header.img_descriptor =0x20;
    output.write((char *)&header, sizeof(header));

    unsigned long nbytes = width * height * bpp;
    // random_data();
    output.write((char *)data, nbytes);

    output.close();

    return true;
}


int Image::set(int x, int y, Pixel p) {
    if (!data || x < 0 || y < 0 || x >= width || y >= height) {
        cerr << "Connot set pixel value." << endl;
        return 0;
    }
    if (p.bpp != bpp) {
        cerr << "Wrong Pixel type." << endl;
    }
    memcpy(data+(x+y*width)*bpp, p.bytes, bpp);
    // data[(x+y*width)*bpp] = value;
    return 1;
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
    bytes = new unsigned char[bpp];
    bytes[0] = v;
}


Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b) : bpp(3) {
    bytes = new unsigned char[bpp];
    bytes[0] = r;
    bytes[1] = g;
    bytes[2] = b;
}


Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : bpp(4) {
    bytes = new unsigned char[bpp];
    bytes[0] = r;
    bytes[1] = g;
    bytes[2] = b;
    bytes[3] = a;
}

Pixel::~Pixel() {
    delete bytes;
}