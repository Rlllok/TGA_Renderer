#pragma once
#include <vector>


class Matrix {
    std::vector<std::vector<float> > m;
    int rows, cols;
public:
    Matrix(int rows=4, int columns=4);
    inline int nrows();
    inline int ncols();

    static Matrix identity(int dimensions);
    std::vector<float>& operator[](const int i);
    Matrix operator*(const Matrix& a);
    Matrix transpose();
    Matrix inverse();
};