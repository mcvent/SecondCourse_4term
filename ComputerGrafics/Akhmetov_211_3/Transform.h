#pragma once
#include "Matrix.h"
#include <math.h>

// Матрица переноса
mat3 translate(float Tx, float Ty) {
    mat3* res = new mat3(1.f); // Единичная матрица
    (*res)[0][2] = Tx;
    (*res)[1][2] = Ty;
    return *res;
}

// Матрица масштабирования (два параметра)
mat3 scale(float Sx, float Sy) {
    mat3* res = new mat3(1.f);
    (*res)[0][0] = Sx;
    (*res)[1][1] = Sy;
    return *res;
}

// Матрица масштабирования (один параметр)
mat3 scale(float S) {
    return scale(S, S);
}

// Матрица поворота
mat3 rotate(float theta) {
    mat3* res = new mat3(1.f);
    (*res)[0][0] = (*res)[1][1] = (float)cos(theta);
    (*res)[0][1] = (float)sin(theta);
    (*res)[1][0] = -(*res)[0][1]; // минус для левой системы координат
    return *res;
}

//зеркальное отражение относительно оси Ox
mat3 mirrorX() {
    mat3 res(1.f);
    res[1][1] = -1; //меняем знак
    return res;
}

//зеркальное отражение относительно оси Oy
mat3 mirrorY() {
    mat3 res(1.f);
    res[0][0] = -1;
    return res;
}