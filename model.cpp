#include "model.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>


Model::Model(const char *filename) : vertices(), faces() {
    std::ifstream infile;
    infile.open(filename, std::ifstream::in);
    if(infile.fail()) return;
    std::string line;

    while (!infile.eof()) {
        std::getline(infile, line);
        std::istringstream iss(line.c_str());
        char tmp;
        if(!line.compare(0, 2, "v ")) {
            iss >> tmp;
            Vector3Float v;
            for (int i = 0; i < 3; i++) iss >> v.raw[i];
            // std::cout << v.raw[1] << " ";
            vertices.push_back(v);
        } else if (!line.compare(0, 3, "vt ")) {
            iss >> tmp >> tmp;
            Vector2Float t;
            for (int i = 0; i < 2; i++) iss >> t.raw[i];
            texture_vertices.push_back(t);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            std::vector<int> t;
            int tid, idx, itmp;
            iss >> tmp;
            while (iss >> idx >> tmp >> tid >> tmp >> itmp) {
                idx--;
                f.push_back(idx);
                tid--;
                t.push_back(tid);
            }
            faces.push_back(f);
            faces_texture.push_back(t);
        }
    }
    std::cout << "Vertecise: " << vertices.size() << std::endl;
    std::cout << "Polygons: " << faces.size() << std::endl;
    std::cout << "Texture V: " << texture_vertices.size() << std::endl;
    std::cout << "Texture F: " << faces_texture.size() << std::endl;
}

Model::~Model() {
}

int Model::getVerticesNum() {
    return (int)vertices.size();
}

int Model::getFacesNum() {
    return (int)faces.size();
}

std::vector<int> Model::face(int idx) {
    return faces[idx];
}

Vector3Float Model::vertex(int i) {
    return vertices[i];
}

Vector2Float Model::textureVertex(int i)
{
    return texture_vertices[i];
}

std::vector<int> Model::faceTexture(int i)
{
    return faces_texture[i];
}