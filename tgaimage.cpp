#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <algorithm>
#include "tgaimage.h"

// IMAGE
Image::Image() : data(NULL), width(0), height(0), bpp(0), zbuffer(NULL)
{

}


Image::Image(unsigned int w, unsigned int h, unsigned int bpp) : width(w), height(h),  bpp(bpp)
{
    unsigned long nbytes = w * h * bpp;
    data = new unsigned char[nbytes];
    memset(data, 0, nbytes);

    zbuffer = new float[w*h];
}


Image::~Image()
{
    if (data) delete [] data;
    if (zbuffer) delete [] zbuffer;
}


unsigned int Image::get_width()
{
    return width;
}


unsigned int Image::get_height()
{
    return height;
}


unsigned int Image::get_bpp()
{
    return bpp;
}


bool Image::read_tga_file(const char *filename) {
    if (data) delete [] data;
    data = NULL;
    std::ifstream in;
    in.open (filename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "can't open file " << filename << "\n";
        in.close();
        return false;
    }
    Header header;
    in.read((char *)&header, sizeof(header));
    if (!in.good()) {
        in.close();
        std::cerr << "an error occured while reading the header\n";
        return false;
    }
    width   = header.img_width;
    height  = header.img_height;
    bpp = header.img_bitspp>>3;
    if (width<=0 || height<=0 || (bpp!=GRAYSCALE_BPP && bpp!=RGB_BPP && bpp!=RGBA_BPP)) {
        in.close();
        std::cerr << "bad bpp (or width/height) value\n";
        return false;
    }
    unsigned long nbytes = bpp*width*height;
    data = new unsigned char[nbytes];
    if (3==header.img_type || 2==header.img_type) {
        in.read((char *)data, nbytes);
        if (!in.good()) {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    } else if (10==header.img_type||11==header.img_type) {
        if (!load_rle_data(in)) {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    } else {
        in.close();
        std::cerr << "unknown file format " << (int)header.img_type << "\n";
        return false;
    }
    if (!(header.img_descriptor & 0x20)) {
        flipVertically();
    }
    if (header.img_descriptor & 0x10) {
        flipHorizontally();
    }
    std::cerr << width << "x" << height << "/" << bpp*8 << "\n";
    in.close();
    return true;
}


bool Image::load_rle_data(std::ifstream &in) {
    unsigned long pixelcount = width*height;
    unsigned long currentpixel = 0;
    unsigned long currentbyte  = 0;
    Pixel colorbuffer;
    do {
        unsigned char chunkheader = 0;
        chunkheader = in.get();
        if (!in.good()) {
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
        if (chunkheader<128) {
            chunkheader++;
            for (int i=0; i<chunkheader; i++) {
                in.read((char *)colorbuffer.bytes, bpp);
                if (!in.good()) {
                    std::cerr << "an error occured while reading the header\n";
                    return false;
                }
                for (int t=0; t<bpp; t++)
                    data[currentbyte++] = colorbuffer.bytes[t];
                currentpixel++;
                if (currentpixel>pixelcount) {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        } else {
            chunkheader -= 127;
            in.read((char *)colorbuffer.bytes, bpp);
            if (!in.good()) {
                std::cerr << "an error occured while reading the header\n";
                return false;
            }
            for (int i=0; i<chunkheader; i++) {
                for (int t=0; t<bpp; t++)
                    data[currentbyte++] = colorbuffer.bytes[t];
                currentpixel++;
                if (currentpixel>pixelcount) {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        }
    } while (currentpixel < pixelcount);
    return true;
}


bool Image::write_tga_file(const char *filename, bool rle) {
    unsigned char developer_area_ref[4] = {0, 0, 0, 0};
    unsigned char extension_area_ref[4] = {0, 0, 0, 0};
    unsigned char footer[18] = {'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0'};
    std::ofstream out;
    out.open (filename, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "can't open file " << filename << "\n";
        out.close();
        return false;
    }
    Header header;
    memset((void *)&header, 0, sizeof(header));
    header.img_bitspp = bpp<<3;
    header.img_width  = width;
    header.img_height = height;
    header.img_type = (bpp==GRAYSCALE_BPP?(rle?11:3):(rle?10:2));
    header.img_descriptor = 0x20; // top-left origin
    out.write((char *)&header, sizeof(header));
    if (!out.good()) {
        out.close();
        std::cerr << "can't dump the tga file\n";
        return false;
    }
    if (!rle) {
        out.write((char *)data, width*height*bpp);
        if (!out.good()) {
            std::cerr << "can't unload raw data\n";
            out.close();
            return false;
        }
    } else {
        if (!unload_rle_data(out)) {
            out.close();
            std::cerr << "can't unload rle data\n";
            return false;
        }
    }
    out.write((char *)developer_area_ref, sizeof(developer_area_ref));
    if (!out.good()) {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write((char *)extension_area_ref, sizeof(extension_area_ref));
    if (!out.good()) {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write((char *)footer, sizeof(footer));
    if (!out.good()) {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.close();
    return true;
}


bool Image::unload_rle_data(std::ofstream &out) {
    const unsigned char max_chunk_length = 128;
    unsigned long npixels = width*height;
    unsigned long curpix = 0;
    while (curpix<npixels) {
        unsigned long chunkstart = curpix*bpp;
        unsigned long curbyte = curpix*bpp;
        unsigned char run_length = 1;
        bool raw = true;
        while (curpix+run_length<npixels && run_length<max_chunk_length) {
            bool succ_eq = true;
            for (int t=0; succ_eq && t<bpp; t++) {
                succ_eq = (data[curbyte+t]==data[curbyte+t+bpp]);
            }
            curbyte += bpp;
            if (1==run_length) {
                raw = !succ_eq;
            }
            if (raw && succ_eq) {
                run_length--;
                break;
            }
            if (!raw && !succ_eq) {
                break;
            }
            run_length++;
        }
        curpix += run_length;
        out.put(raw?run_length-1:run_length+127);
        if (!out.good()) {
            std::cerr << "can't dump the tga file\n";
            return false;
        }
        out.write((char *)(data+chunkstart), (raw?run_length*bpp:bpp));
        if (!out.good()) {
            std::cerr << "can't dump the tga file\n";
            return false;
        }
    }
    return true;
}


bool Image::flipVertically()
{
    if (!data) return false;
    unsigned long bytesperline = width*bpp;
    unsigned char line[bytesperline];
    int halfheight = height>>1;
    for (int i = 0; i < halfheight; i++) {
        unsigned long idx1 = i * bytesperline;
        unsigned long idx2 = (height-1-i) * bytesperline;
        memmove((void*)line,(void*)(data+idx1), bytesperline);
        memmove((void*)(data+idx1), (void*)(data+idx2), bytesperline);
        memmove((void*)(data+idx2), (void*)line, bytesperline);
    }
    return true;
}


bool Image::flipHorizontally() {
    if (!data) return false;
    int half = width>>1;
    for (int i=0; i<half; i++) {
        for (int j=0; j<height; j++) {
            Pixel c1 = getPixel(i, j);
            Pixel c2 = getPixel(width-1-i, j);
            setPixel(i, j, c2);
            setPixel(width-1-i, j, c1);
        }
    }
    return true;
}


bool Image::setPixel(int x, int y, Pixel p)
{
    if (!data || x < 0 || y < 0 || x >= width || y >= height) {
        cerr << "Cannot set pixel value." << endl;
        throw invalid_argument("Wrong argument.");
        return false;
    }
    if (p.get_bpp() != bpp) {
        std::cout << "Pixel bpp: " << p.get_bpp() << " " << "Image bpp: " << bpp << std::endl;
        throw invalid_argument("Wrong Pixel Type.");
    }
    memcpy(data+(x+y*width)*bpp, p.get_bytes(), bpp);
    return true;
}


Pixel Image::getPixel(int x, int y) {
    if (!data || x < 0 || y < 0 || x >= width || y >= height) {
        std::cout << "x: " << x << " " << "y: " << y << std::endl;
        cerr << "Cannot set pixel value." << endl;
        return Pixel();
    }
    return Pixel(data+(x+y*width)*bpp, bpp);
}

// PIXEL 
Pixel::Pixel() : bpp(0) {}


Pixel::Pixel(unsigned char v) : bpp(1)
{
    bytes[0] = v;
    bytes[1] = 0;
    bytes[2] = 0;
    bytes[3] = 0;
}


Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b) : bpp(3)
{
    bytes[0] = b;
    bytes[1] = g;
    bytes[2] = r;
    bytes[3] = 0;
}


Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : bpp(4)
{
    bytes[0] = b;
    bytes[1] = g;
    bytes[2] = r;
    bytes[3] = a;
}


Pixel::Pixel(const unsigned char *p, unsigned char bpp) : bpp(bpp) {
    for (int i=0; i<(int)bpp; i++) {
        bytes[i] = p[i];
    }
    for (int i=bpp; i<4; i++) {
        bytes[i] = 0;
    }
}


int Pixel::get_bpp()
{
    return this->bpp;
}


unsigned char* Pixel::get_bytes()
{
    return this->bytes;
}


Pixel Pixel::operator *(float num)
{
    for (int i = 0; i < 4; i++) {
        bytes[i] = std::max(0, std::min(int(bytes[i] * num), 255));
    }
    return *this;
}