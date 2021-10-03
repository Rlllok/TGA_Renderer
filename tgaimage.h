#pragma once

#include <fstream>


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
    const int bpp;
    unsigned char* bytes;

    Pixel();
    Pixel(unsigned char v); // bpp = 1
    Pixel(unsigned char r, unsigned char g, unsigned char b); // bpp = 3
    Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a); // bpp = 4
    ~Pixel();
};


class Image {
protected:
    unsigned char* data;
    unsigned int width;
    unsigned int height;
    unsigned int bpp;

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
    bool write_file(const char* filename);
    int set(int x, int y, Pixel p);

    void random_data(); // for testing
};