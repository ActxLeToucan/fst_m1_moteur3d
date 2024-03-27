#pragma once

class Triangle {
public:
    /**
     * @brief The indices of the vertices of the triangle
     * @details The indices are in the range [1, n]
     */
    int v1, v2, v3;
    /**
     * @brief The indices of the texture coordinates of the triangle
     * @details The indices are in the range [1, n]
     */
    int vt1, vt2, vt3;
    /**
     * @brief The indices of the normals of the triangle
     * @details The indices are in the range [1, n]
     */
    int vn1, vn2, vn3;

    /**
     * @brief Construct a new Triangle with no vertices, texture coordinates, or normals
     */
    Triangle() = default;
};
