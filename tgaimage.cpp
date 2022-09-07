#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <time.h>
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
    resetZbuffer();
}


Image::~Image()
{
    if (data) delete [] data;
    if (zbuffer) delete [] zbuffer;
}


// bool Image::write_file(const char* filename)
// {
//     ofstream output;

//     output.open(filename, ios::binary);

//     if (!output.is_open()) {
//         cerr << "can't open the file." << endl;
//         output.close();
//         return false;
//     }

//     Header header = {};
//     memset((void *)&header, 0, sizeof(header));
//     header.img_bitspp = bpp<<3; // multiply bpp by 8
//     header.img_width = width;
//     header.img_height = height;
//     header.img_type = (bpp==GRAYSCALE_BPP?3:2);
//     header.img_descriptor = 0x20;
//     output.write((char*)&header, sizeof(header));

//     unsigned long nbytes = width * height * bpp;
//     output.write((char*)data, nbytes);

//     output.close();

//     return true;
// }


// bool Image::read_file(const char *filename)
// {
//     if (data) delete [] data;
//     ifstream input_file;
//     input_file.open(filename, ios::binary);

//     if (!input_file.is_open()) {
//         cerr << "Cannot open file." << endl;
//         input_file.close();
//         return false;
//     }

//     Header header;
//     input_file.read((char*)&header, sizeof(header));
//     if (!input_file.good()) {
//         cerr << "Cannot read the header." << endl;
//         input_file.close();
//         return false;
//     }

//     width = header.img_width;
//     height = header.img_height;
//     bpp = header.img_bitspp>>3;
//     if (width<=0 || height <=0 || (bpp!=GRAYSCALE_BPP && bpp!=RGB_BPP && bpp!=RGBA_BPP)) {
//         cerr << "Wrong height, width or bytes per pixel." << endl;
//         input_file.close();
//         return false;
//     }

//     unsigned long nbytes = bpp*width*height;
//     data = new unsigned char[nbytes];
//     if (header.img_type==3 || header.img_type==2) {
//         input_file.read((char*)data, nbytes);
//         if (!input_file.good()) {
//             cerr << "Cannot read the data." << endl;
//             input_file.close();
//             return false;
//         }
//     } else {
//         cerr << "Unknown data format." << endl;
//         input_file.close();
//         return false;
//     }

//     return true;
// }


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
            Pixel c1 = (i, j);
            Pixel c2 = get(width-1-i, j);
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
        throw invalid_argument("Wrong Pixel Type.");
    }
    memcpy(data+(x+y*width)*bpp, p.get_bytes(), bpp);
    return true;
}


Pixel Image::get(int x, int y) {
    if (!data || x<0 || y<0 || x>=width || y>=height) {
        return Pixel();
    }
    return Pixel(data+(x+y*width)*bpp, bpp);
}


void Image::drawLine(int x0, int y0, int x1, int y1, Pixel pixel)
{
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

// Draws triangle on the screen using Bacycentric coordinates and Box arround triangle.
// This is more for education purpose.
void Image::drawTriangle(Vector2Int v1, Vector2Int v2, Vector2Int v3, Pixel pixel)
{
    // If don't draw tringle, it lefts some pixels.
    drawLine(v1.x, v1.y, v2.x, v2.y, pixel);
    drawLine(v2.x, v2.y, v3.x, v3.y, pixel);
    drawLine(v3.x, v3.y, v1.x, v1.y, pixel);

    Vector2Int triangle[3] = {v1, v2, v3};

    Vector2Int minPoint = getTriangleBounderyMinPoint(triangle);
    Vector2Int maxPoint = getTriangleBounderyMaxPoint(triangle);

    Vector2Int point(0, 0);

    for (point.x = minPoint.x; point.x <= maxPoint.x; point.x++) {
        for (point.y = minPoint.y; point.y <= maxPoint.y; point.y++) {
            Vector3Float barycentricCord = getBarycentricCoordinates(point, triangle);
            if (barycentricCord.x < 0 || barycentricCord.y < 0 || barycentricCord.z < 0)
                continue;
            
            setPixel(point.x, point.y, pixel);
        }
    }
}


void Image::drawTrigon(Vector3Float v1, Vector3Float v2, Vector3Float v3, Pixel pixel)
{
    v1 = Vector3Float(int((v1.x+1.)*width/2.+.5), int((v1.y+1.)*height/2.+.5), v1.z);
    v2 = Vector3Float(int((v2.x+1.)*width/2.+.5), int((v2.y+1.)*height/2.+.5), v2.z);
    v3 = Vector3Float(int((v3.x+1.)*width/2.+.5), int((v3.y+1.)*height/2.+.5), v3.z);

    Vector3Float triangle[3] = {v1, v2, v3};

    Vector2Float minPoint( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vector2Float maxPoint(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vector2Float clamp(width - 1, height - 1);
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            minPoint.raw[j] = std::max(0.f, std::min(minPoint.raw[j], triangle[i].raw[j]));
            maxPoint.raw[j] = std::min(clamp.raw[j], std::max(maxPoint.raw[j], triangle[i].raw[j]));
        }
    }

    Vector3Float point;

    for (point.x = minPoint.x; point.x <= maxPoint.x; point.x++) {
        for (point.y = minPoint.y; point.y <= maxPoint.y; point.y++) {
            Vector3Float barycentricCord = getBarycentricCoordinates(point, triangle);
            if (barycentricCord.x < 0 || barycentricCord.y < 0 || barycentricCord.z < 0) continue;
            
            point.z = 0;
            for (int i = 0; i < 3; i++) {
                point.z += triangle[i].z * barycentricCord.raw[i];
            }
            if (zbuffer[int(point.x + point.y*width)] < point.z) {
                zbuffer[int(point.x + point.y*width)] = point.z;
                setPixel(point.x, point.y, pixel);
            }
        }
    }
}


void Image::resetZbuffer()
{
    if (!zbuffer) return;

    for (int i = height*width; i--; zbuffer[i] = -std::numeric_limits<float>::max());
}


Vector3Float Image::getBarycentricCoordinates(Vector2Int point, Vector2Int triangle[3])
{
    float det = (triangle[1].y - triangle[2].y)*(triangle[0].x - triangle[2].x)
        + (triangle[2].x - triangle[1].x)*(triangle[0].y - triangle[2].y);
    float lambda1 = (triangle[1].y - triangle[2].y)*(point.x - triangle[2].x)
        + (triangle[2].x - triangle[1].x)*(point.y - triangle[2].y);
    lambda1 = lambda1 / det;
    float lambda2 = (triangle[2].y - triangle[0].y)*(point.x - triangle[2].x)
        + (triangle[0].x - triangle[2].x)*(point.y - triangle[2].y);
    lambda2 = lambda2 / det;
    float lambda3 = 1 - lambda1 - lambda2;

    return Vector3Float(lambda1, lambda2, lambda3);
}


Vector3Float Image::getBarycentricCoordinates(Vector3Float point, Vector3Float triangle[3])
{
    Vector3Float s[2];

    // s1 contains x values
    // s2 contains y values
    for (int i = 0; i < 2; i++) {
        s[i].raw[0] = triangle[2].raw[i] - triangle[0].raw[i]; // C - A vector
        s[i].raw[1] = triangle[1].raw[i] - triangle[0].raw[i]; // B - A vector
        s[i].raw[2] = triangle[0].raw[i] - point.raw[i]; // A - P vector
    }

    // barycentricCoord.x is triangle with B - A and A - P
    // barycentricCoord.y is triangle with C - A and A - P
    // barycentricCoord.z is triangle with C - A and B - A
    Vector3Float barycentricCoord = s[0].crossProduct(s[1]);
    if (std::abs(barycentricCoord.raw[2]) > 1e-2) {
        return Vector3Float(
            1.f - (barycentricCoord.x + barycentricCoord.y) / barycentricCoord.z,
            barycentricCoord.y / barycentricCoord.z,
            barycentricCoord.x / barycentricCoord.z
        );
    }

    return Vector3Float(-1, 1, 1);
}


Vector2Int Image::getTriangleBounderyMinPoint(Vector2Int triangle[3])
{
    Vector2Int minPoint(width - 1, height - 1);
    
    for (int i = 0; i < 3; i++) {
        minPoint.x = std::max(0, std::min(minPoint.x, triangle[i].x));
        minPoint.y = std::max(0, std::min(minPoint.y, triangle[i].y));
    }

    return minPoint;
}


Vector2Int Image::getTriangleBounderyMaxPoint(Vector2Int triangle[3])
{
    Vector2Int maxPoint(0, 0);
    Vector2Int clap(width - 1, height - 1);

    for (int i = 0; i < 3; i++) {
        maxPoint.x = std::min(std::max(maxPoint.x, triangle[i].x), clap.x);
        maxPoint.y = std::min(std::max(maxPoint.y, triangle[i].y), clap.y);
    }
    
    return maxPoint;
}

// PIXEL 
Pixel::Pixel() : bpp(0) {}


Pixel::Pixel(unsigned char v) : bpp(1)
{
    bytes[0] = v;
}


Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b) : bpp(3)
{
    bytes[0] = b;
    bytes[1] = g;
    bytes[2] = r;
}


Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : bpp(4)
{
    bytes[0] = b;
    bytes[1] = g;
    bytes[2] = r;
    bytes[3] = a;
}


Pixel::Pixel(const unsigned char *p, unsigned char bpp) : bytes(), bpp(bpp) {
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