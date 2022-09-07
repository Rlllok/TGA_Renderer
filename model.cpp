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
            vertices.push_back(v);
        } else if (!line.compare(0, 3, "vt ")) {
            // Texture
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            int itmp, idx;
            iss >> tmp;
            while (iss >> idx >> tmp >> itmp >> tmp >> itmp) {
                idx--;
                f.push_back(idx);
            }
            faces.push_back(f);
        }
    }
    std::cout << vertices.size() << " " << faces.size() << std::endl;
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