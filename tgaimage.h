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
    bool write_tga_file(const char* filename, bool rle = true);
    bool read_tga_file(const char* filename);
    bool load_rle_data(std::ifstream &in);
    bool unload_rle_data(std::ofstream &out);
    bool flipVertically();
    bool flipHorizontally();
    bool setPixel(int x, int y, Pixel p);
    Pixel get(int x, int y);
    void drawLine(int x0, int y0, int x1, int y1, Pixel pixel);
    void drawTriangle(Vector2Int v1, Vector2Int v2, Vector2Int v3, Pixel pixel);
    void drawTrigon(Vector3Float v1, Vector3Float v2, Vector3Float v3, Pixel pixel);
    void resetZbuffer();
    Vector3Float getBarycentricCoordinates(Vector2Int point, Vector2Int triangle[3]);
    Vector3Float getBarycentricCoordinates(Vector3Float point, Vector3Float triangle[3]);
    Vector2Int getTriangleBounderyMinPoint(Vector2Int triangle[3]);
    Vector2Int getTriangleBounderyMaxPoint(Vector2Int triangle[3]);
};