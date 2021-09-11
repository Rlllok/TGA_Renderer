#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <time.h>

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
    unsigned char img_bpp ;
    unsigned char img_descriptor;
};
#pragma pack(pop)

int main() {
    //header data

    int width = 100;
    int height = 100;
    int bpp = 1;

    unsigned long nbytes = width*height*bpp;
    unsigned char* data = new unsigned char[nbytes];
    srand(time(NULL));
    for (int i = 0; i < nbytes; i++) {
        data[i] = rand()%255;
    }

    unsigned char developer_area_ref[4] = {0, 0, 0, 0};
	unsigned char extension_area_ref[4] = {0, 0, 0, 0};
	unsigned char footer[18] = {'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0'};



    ofstream out;
    
    out.open("test.tga", ios::binary);
    
    if (!out.is_open()) {
        cerr << "can't open file." << endl;
        out.close();
        return 0;
    }

    //header
    // out.write((char *)id_lenght, sizeof(id_lenght));
    // out.write((char *)colormap_type, sizeof(colormap_type));
    // out.write((char *)img_type, sizeof(img_type));
    // out.write((char *)colormap_origin, sizeof(colormap_origin));
    // out.write((char *)colormap_lenght, sizeof(colormap_lenght));
    // out.write((char *)colormap_bpp, sizeof(colormap_bpp));
    // out.write((char *)x_origin, sizeof(x_origin));
    // out.write((char *)y_origin, sizeof(y_origin));
    // out.write((char *)img_width, sizeof(img_width));
    // out.write((char *)img_height, sizeof(img_height));
    // out.write((char *)img_bpp, sizeof(img_bpp));
    // out.write((char *)img_descriptor, sizeof(img_descriptor));
    Header header;
    memset((void *)&header, 0, sizeof(header));
    header.img_bpp = bpp<<3;
    header.img_width = width;
    header.img_height = height;
    header.img_type = 3;
    header.img_descriptor = 0x20;
    out.write((char *)&header, sizeof(header));

    //data
    out.write((char *)data, nbytes);

    //developer
    out.write((char *)developer_area_ref, sizeof(developer_area_ref));

    //extension
    out.write((char *)extension_area_ref, sizeof(extension_area_ref));

    //footer
    out.write((char *)footer, sizeof(footer));

    out.close();


    return 0;
}