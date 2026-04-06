#pragma once
#include "Matrix.h"
#include <vector>

class path {
public:
    std::vector<vec2> vertices; // точки ломаной
    vec3 color;                  // цвет (R,G,B)
    float thickness;             // толщина

    path(std::vector<vec2> verts, vec3 col, float thickn) {
        vertices = verts;
        color = col;
        thickness = thickn;
    }
};