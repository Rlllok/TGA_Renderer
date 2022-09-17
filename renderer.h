#pragma once

#include "model.h"
#include "tgaimage.h"
#include "vectors.h"
#include "matrix.h"

class Renderer
{
private:
    Image* output;
    Model* model;
    Image* texture;
    float* zbuffer;
    const int depth = 255;

public:
    Renderer(Model* _model, Image* _output, Image* _texture = nullptr);
    void drawLine(int x0, int y0, int x1, int y1, Pixel pixel);
    void drawTriangle(Vector2Int v1, Vector2Int v2, Vector2Int v3, Pixel pixel);
    void drawTrigon(Vector3Float trigon[3], Pixel pixel);
    void drawTrigon(Vector3Float trigon[3], Vector2Float tv[3], float lightIntensity);
    void resetZbuffer();
    void render();
    bool writeFile(const char* name);
    Vector3Float getBarycentricCoordinates(Vector2Int point, Vector2Int triangle[3]);
    Vector3Float getBarycentricCoordinates(Vector3Float point, Vector3Float triangle[3]);
    Vector2Int getTriangleBounderyMinPoint(Vector2Int triangle[3]);
    Vector2Int getTriangleBounderyMaxPoint(Vector2Int triangle[3]);
    Vector2Int getTextureCoordinates(Vector2Float t);
    Matrix getViewport(int x, int y, int w, int h);
    Vector3Float m2v(Matrix m) {
        return Vector3Float(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
    }
};