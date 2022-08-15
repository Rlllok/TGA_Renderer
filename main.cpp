#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <algorithm>
#include <cmath>
#include "tgaimage.h"
#include "model.h"


using namespace std;


int main() {
    Model* model = new Model("head.obj");
    const int width = 800;
    const int height = 600;
    Image image(width, height, Image::RGB_BPP);
    Pixel white(255,255,255);

    for (int i=0; i<model->getFacesNum(); i++) {
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++) {
            Vector3Float v0 = model->vertex(face[j]);
            Vector3Float v1 = model->vertex(face[(j+1)%3]);
            int x0 = (v0.x+1.)*(width-1)/2.;
            int y0 = (v0.y+1.)*(height-1)/2.;
            int x1 = (v1.x+1.)*(width-1)/2.;
            int y1 = (v1.y+1.)*(height-1)/2.;
            image.drawLine(x0, y0, x1, y1, white);
        }
    }

    image.write_file("output.tga");

    return 0;
}