#include "renderer.h"
#include "matrix.h"
#include <algorithm>
#include <iostream>


Renderer::Renderer(Model* _model, Image* _output, Image* _texture)
{
    model = _model;
    output = _output;
    texture = _texture;
    zbuffer = new float[output->get_width()*output->get_height()];
    resetZbuffer();
}


void Renderer::drawLine(int x0, int y0, int x1, int y1, Pixel pixel)
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
            output->setPixel(y, x, pixel);
        else
            output->setPixel(x, y, pixel);
        error += dy;
        if ((error << 1) >= dx) {
            y += y_step;
            error -= dx;
        }
    }
}

// Draws triangle on the screen using Bacycentric coordinates and Box arround triangle.
// This is more for education purpose.
void Renderer::drawTriangle(Vector2Int v1, Vector2Int v2, Vector2Int v3, Pixel pixel)
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
            output->setPixel(point.x, point.y, pixel);
        }
    }
}


void Renderer::drawTrigon(Vector3Float trigon[3], Pixel pixel)
{
    Vector2Float minPoint( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vector2Float maxPoint(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vector2Float clamp(output->get_width() - 1, output->get_height() - 1);
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            minPoint.raw[j] = std::max(0.f, std::min(minPoint.raw[j], trigon[i].raw[j]));
            maxPoint.raw[j] = std::min(clamp.raw[j], std::max(maxPoint.raw[j], trigon[i].raw[j]));
        }
    }

    Vector3Float point;

    for (point.x = minPoint.x; point.x <= maxPoint.x; point.x++) {
        for (point.y = minPoint.y; point.y <= maxPoint.y; point.y++) {
            Vector3Float barycentricCord = getBarycentricCoordinates(point, trigon);
            if (barycentricCord.x < 0 || barycentricCord.y < 0 || barycentricCord.z < 0) continue;
            
            point.z = 0;
            for (int i = 0; i < 3; i++) {
                point.z += trigon[i].z * barycentricCord.raw[i];
            }
            if (zbuffer[int(point.x + point.y*output->get_width())] < point.z) {
                zbuffer[int(point.x + point.y*output->get_width())] = point.z;
                output->setPixel(point.x, point.y, pixel);
            }
        }
    }
}


void Renderer::drawTrigon(Vector3Float trigon[3], Vector2Float tv[3], float lightIntensity)
{
    if (trigon[0].y==trigon[1].y && trigon[0].y==trigon[2].y) return; // i dont care about degenerate triangles
    if (trigon[1].y>trigon[1].y) { std::swap(trigon[0], trigon[1]); std::swap(tv[0], tv[1]); }
    if (trigon[0].y>trigon[2].y) { std::swap(trigon[0], trigon[2]); std::swap(tv[0], tv[2]); }
    if (trigon[1].y>trigon[2].y) { std::swap(trigon[1], trigon[2]); std::swap(tv[1], tv[2]); }

    Vector2Float minPoint( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vector2Float maxPoint(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vector2Float clamp(output->get_width() - 1, output->get_height() - 1);
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            minPoint.raw[j] = std::max(0.f, std::min(minPoint.raw[j], trigon[i].raw[j]));
            maxPoint.raw[j] = std::min(clamp.raw[j], std::max(maxPoint.raw[j], trigon[i].raw[j]));
        }
    }

    Vector3Float point;

    for (point.x = minPoint.x; point.x <= maxPoint.x; point.x++) {
        for (point.y = minPoint.y; point.y <= maxPoint.y; point.y++) {
            Vector3Float barycentricCord = getBarycentricCoordinates(point, trigon);
            if (barycentricCord.x < 0 || barycentricCord.y < 0 || barycentricCord.z < 0) continue;
            
            point.z = 0;
            Vector2Float textureCoord(0, 0);
            for (int i = 0; i < 3; i++) {
                point.z += trigon[i].z * barycentricCord.raw[i];
                textureCoord.x += tv[i].x * barycentricCord.raw[i];
                textureCoord.y += tv[i].y * barycentricCord.raw[i];
            }

            if (zbuffer[int(point.x + point.y*output->get_width())] < point.z) {
                zbuffer[int(point.x + point.y*output->get_width())] = point.z;
                Vector2Int t = getTextureCoordinates(textureCoord);
                output->setPixel(point.x, point.y, texture->getPixel(t.x, t.y) * lightIntensity);
            }
        }
    }
}


void Renderer::resetZbuffer()
{
    if (!zbuffer) return;

    for (int i = output->get_height()*output->get_width(); i--; zbuffer[i] = -std::numeric_limits<float>::max());
}


void Renderer::render()
{
    Vector3Float lightDirection(0, 0, -1);
    Vector3Float cameraVector(0, 0, 3);
    Matrix viewPort = getViewport(
        output->get_width()/8, 
        output->get_height()/8,
        output->get_width()*3/4,
        output->get_height()*3/4
    );
    Matrix projectionMatrix = Matrix::identity(4);
    projectionMatrix[3][2] = -1.f / cameraVector.z;

    for (int i=0; i < model->getFacesNum(); i++) {
        std::vector<int> face = model->face(i);
        std::vector<int> face_texture = model->faceTexture(i);
        Vector3Float screenCoords[3];
        Vector3Float worldCoords[3];
        Vector2Float textureCoords[3];
        for (int j=0; j<3; j++) {
            Vector3Float v = model->vertex(face[j]);
            Matrix vMatrix(4,1);
            vMatrix[0][0] = v.x;
            vMatrix[1][0] = v.y;
            vMatrix[2][0] = v.z;
            vMatrix[3][0] = 1;
            
            // screenCoords[j] = m2v(viewPort*projectionMatrix*vMatrix);
            screenCoords[j] = Vector3Float(int((v.x+1.)*output->get_width()/2.+.5), int((v.y+1.)*output->get_height()/2.+.5), v.z);
            worldCoords[j] = v;
            textureCoords[j] = model->textureVertex(face_texture[j]);
        }

        Vector3Float n = (worldCoords[2] - worldCoords[0])^(worldCoords[1] - worldCoords[0]);
        n.normalize();
        float lightIntensity = n * lightDirection;

        if (lightIntensity > 0) {
            drawTrigon(
                screenCoords,
                textureCoords,
                lightIntensity
            );
        }
    }
}


bool Renderer::writeFile(const char* name)
{
    output->flipVertically();
    return output->write_tga_file(name);
}


Vector3Float Renderer::getBarycentricCoordinates(Vector2Int point, Vector2Int triangle[3])
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


Vector3Float Renderer::getBarycentricCoordinates(Vector3Float point, Vector3Float triangle[3])
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


Vector2Int Renderer::getTriangleBounderyMinPoint(Vector2Int triangle[3])
{
    Vector2Int minPoint(output->get_width() - 1, output->get_height() - 1);
    
    for (int i = 0; i < 3; i++) {
        minPoint.x = std::max(0, std::min(minPoint.x, triangle[i].x));
        minPoint.y = std::max(0, std::min(minPoint.y, triangle[i].y));
    }

    return minPoint;
}


Vector2Int Renderer::getTriangleBounderyMaxPoint(Vector2Int triangle[3])
{
    Vector2Int maxPoint(0, 0);
    Vector2Int clap(output->get_width() - 1, output->get_height() - 1);

    for (int i = 0; i < 3; i++) {
        maxPoint.x = std::min(std::max(maxPoint.x, triangle[i].x), clap.x);
        maxPoint.y = std::min(std::max(maxPoint.y, triangle[i].y), clap.y);
    }
    
    return maxPoint;
}


Vector2Int Renderer::getTextureCoordinates(Vector2Float t)
{
    return Vector2Int(int((t.x)*texture->get_width()), int((t.y)*texture->get_height()));
}


Matrix Renderer::getViewport(int x, int y, int w, int h)
{
    Matrix m = Matrix::identity(4);
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = depth/2.f;
    return m;
}