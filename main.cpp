#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <algorithm>
#include <cmath>

#include "tgaimage.h"
#include "model.h"
#include "vectors.h"


using namespace std;


int main() {
    Model* model = new Model("head.obj");
    const int width = 800;
    const int height = 600;
    srand(time(NULL));

// FRAME
    // Image image(width, height, Image::RGB_BPP);
    // Pixel white(255,255,255);

    // for (int i=0; i<model->getFacesNum(); i++) {
    //     std::vector<int> face = model->face(i);
    //     for (int j=0; j<3; j++) {
    //         Vector3Float v0 = model->vertex(face[j]);
    //         Vector3Float v1 = model->vertex(face[(j+1)%3]);
    //         int x0 = (v0.x+1.)*(width-1)/2.;
    //         int y0 = (v0.y+1.)*(height-1)/2.;
    //         int x1 = (v1.x+1.)*(width-1)/2.;
    //         int y1 = (v1.y+1.)*(height-1)/2.;
    //         image.drawLine(x0, y0, x1, y1, white);
    //     }
    // }

    // image.flipVertically();
    // image.write_file("frame.tga");

// TRIANGLE
    // Image image(width, height, Image::RGB_BPP);
    // Pixel white(255,255,255);

    // Vector2Int t0[3] = {Vector2Int(10, 70),   Vector2Int(50, 160),  Vector2Int(70, 80)}; 
    // Vector2Int t1[3] = {Vector2Int(180, 50),  Vector2Int(150, 1),   Vector2Int(70, 180)}; 
    // Vector2Int t2[3] = {Vector2Int(180, 150), Vector2Int(120, 160), Vector2Int(130, 180)};

    // image.drawTriangle(t0[0], t0[1], t0[2], white);
    // // image.drawTriangle(t1[0], t1[1], t1[2], white);
    // // image.drawTriangle(t2[0], t2[1], t2[2], white);

    // image.flipVertically();
    // image.write_file("triangle.tga");

// Head Polygons
    // Image image(width, height, Image::RGB_BPP);
    // Pixel white(255,255,255);

    // Vector3Float lightDirection(0, 0, -1);

    // for (int i=0; i<model->getFacesNum(); i++) {
    //     std::vector<int> face = model->face(i);
    //     Vector2Int screenCoords[3];
    //     Vector3Float worldCoords[3];
    //     for (int j=0; j<3; j++) {
    //         Vector3Float v = model->vertex(face[j]);
    //         screenCoords[j] = Vector2Int((v.x+1)*(width-1)/2., (v.y+1)*(height-1)/2);
    //         worldCoords[j] = v;
    //     }

    //     Vector3Float n = (worldCoords[2] - worldCoords[0])^(worldCoords[1] - worldCoords[0]);
    //     n.normalize();
    //     float lightIntensity = n * lightDirection;

    //     if (lightIntensity > 0) {
    //         image.drawTriangle(
    //             screenCoords[0],
    //             screenCoords[1],
    //             screenCoords[2],
    //             Pixel(lightIntensity * 255, lightIntensity * 255, lightIntensity * 255)
    //         );
    //     }
    // }

    // image.flipVertically();
    // image.write_file("Outputs/triangles.tga");

    // Z-BUFFER
    // Image image(width, height, Image::RGB_BPP);
    // Pixel white(255,255,255);

    // Vector3Float lightDirection(0, 0, -1);

    // for (int i=0; i<model->getFacesNum(); i++) {
    //     std::vector<int> face = model->face(i);
    //     Vector2Int screenCoords[3];
    //     Vector3Float worldCoords[3];
    //     for (int j=0; j<3; j++) {
    //         Vector3Float v = model->vertex(face[j]);
    //         screenCoords[j] = Vector2Int((v.x+1)*(width-1)/2., (v.y+1)*(height-1)/2);
    //         worldCoords[j] = v;
    //     }

    //     Vector3Float n = (worldCoords[2] - worldCoords[0])^(worldCoords[1] - worldCoords[0]);
    //     n.normalize();
    //     float lightIntensity = n * lightDirection;

    //     if (lightIntensity > 0) {
    //         image.drawTrigon(
    //             worldCoords[0],
    //             worldCoords[1],
    //             worldCoords[2],
    //             Pixel(lightIntensity * 255, lightIntensity * 255, lightIntensity * 255)
    //         );
    //     }
    // }

    // image.flipVertically();
    // image.write_file("Outputs/zbuffer.tga");

// TEXTURE
    Image image(width, height, Image::RGB_BPP);
    Pixel white(255,255,255);

    Image texture;
    texture.read_tga_file("head_texture.tga");

    Vector3Float lightDirection(0, 0, -1);

    for (int i=0; i<model->getFacesNum(); i++) {
        std::vector<int> face = model->face(i);
        Vector2Int screenCoords[3];
        Vector3Float worldCoords[3];
        for (int j=0; j<3; j++) {
            Vector3Float v = model->vertex(face[j]);
            screenCoords[j] = Vector2Int((v.x+1)*(width-1)/2., (v.y+1)*(height-1)/2);
            worldCoords[j] = v;
        }

        Vector3Float n = (worldCoords[2] - worldCoords[0])^(worldCoords[1] - worldCoords[0]);
        n.normalize();
        float lightIntensity = n * lightDirection;

        if (lightIntensity > 0) {
            image.drawTrigon(
                worldCoords[0],
                worldCoords[1],
                worldCoords[2],
                Pixel(lightIntensity * 255, lightIntensity * 255, lightIntensity * 255)
            );
        }
    }

    image.flipVertically();
    image.write_tga_file("Outputs/withTexture.tga");

    return 0;
}