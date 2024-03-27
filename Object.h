#pragma once

#include <vector>

#include "tgaimage.h"
#include "Triangle.h"
#include "glm/vec3.hpp"

class Object {
public:
    /**
     * @brief The points of the object
     */
    std::vector<glm::vec3> points;
    /**
     * @brief The texture coordinates of the object
     */
    std::vector<glm::vec3> textureCoords;
    /**
     * @brief The normals of the object
     */
    std::vector<glm::vec3> normals;
    /**
     * @brief The triangles of the object
     */
    std::vector<Triangle> triangles;
    /**
     * @brief The texture of the object
     */
    TGAImage texture;

    /**
     * @brief Construct a new Object with no points or triangles
     */
    Object() = default;

    /**
     * @brief Construct a new Object from a file
     * @param file The file to read from
     * @param texture The texture image
     * @throws std::runtime_error If the file is not in the correct format
     */
    explicit Object(std::ifstream &file, TGAImage &texture);
};