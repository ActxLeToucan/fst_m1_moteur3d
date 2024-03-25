#pragma once

class Triangle {
public:
    /**
     * @brief The vertices of the triangle
     * @details The vertices are in the range [1, n]
     */
    int v1, v2, v3;
    /**
     * @brief The texture coordinates of the triangle
     * @details The texture coordinates are in the range [1, n]
     */
    int vt1, vt2, vt3;
    /**
     * @brief The normals of the triangle
     * @details The normals are in the range [1, n]
     */
    int vn1, vn2, vn3;

    /**
     * @brief Construct a new Triangle with no vertices, texture coordinates, or normals
     */
    Triangle() = default;
};
