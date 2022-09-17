#pragma once

#include <fstream>
#include "vectors.h"

using namespace std;


#pragma pack(push,1)
struct Header {
    unsigned char id_lenght;
    unsigned char colormap_type;
    unsigned char img_type;
    unsigned short colormap_origin;
    unsigned short colormap_lenght;
    unsigned char colormap_bpp;
    unsigned short x_origin;
    unsigned short y_origin;
    unsigned short img_width;
    unsigned short img_height;
    unsigned char img_bitspp;
    unsigned char img_descriptor;
};
#pragma pack(pop)


class Pixel {
public:
    int bpp;
    unsigned char bytes[4];

    Pixel();
    Pixel(unsigned char v); // bpp = 1
    Pixel(unsigned char r, unsigned char g, unsigned char b); // bpp = 3
    Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a); // bpp = 4
    Pixel(const unsigned char *p, unsigned char bpp);
    int get_bpp();
    unsigned char* get_bytes();
    Pixel operator *(float num);
};


class Image {
private:
    unsigned char* data;
    unsigned int width;
    unsigned int height;
    unsigned int bpp;
    float* zbuffer;

public:
    enum Image_types {
        GRAYSCALE_IMG = 3,
        RGB_IMG = 2,
    };

    enum BPP_types {
        GRAYSCALE_BPP = 1,
        RGB_BPP = 3,
        RGBA_BPP = 4,
    };

    Image();
    Image(unsigned int width, unsigned int height, unsigned int bpp);
    ~Image();
    Image& operator=(const Image& img);
    unsigned int get_width();
    unsigned int get_height();
    unsigned int get_bpp();
    bool write_tga_file(const char* filename, bool rle = true);
    bool read_tga_file(const char* filename);
    bool load_rle_data(std::ifstream &in);
    bool unload_rle_data(std::ofstream &out);
    bool flipVertically();
    bool flipHorizontally();
    bool setPixel(int x, int y, Pixel p);
    Pixel getPixel(int x, int y);
};