#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "tgaimage.h"

using namespace std;

int main() {
    Image image(100, 100, Image::RGBA_BPP);
    image.set(50, 50, Pixel(123,255,45,255));
    image.write_file("test.tga");

    cout << "DONE!" << endl;
    return 0;
}