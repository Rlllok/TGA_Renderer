#pragma once

#include <vector>
#include "vectors.h"

class Model {
private:
    std::vector<Vector3Float> vertices;
    std::vector<std::vector<int>> faces;
    std::vector<Vector3Float> textures;

public:
    Model(const char *filename);
    ~Model();
    int getVerticesNum();
    int getFacesNum();
    Vector3Float vertex(int i);
    std::vector<int> face(int i);
    Vector3Float texture(int i);
};