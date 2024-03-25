#pragma once

#include <vector>
#include "Triangle.h"
#include "glm/vec3.hpp"

class Object {
public:
    /**
     * @brief The points of the object
     */
    std::vector<glm::vec3> points;
    /**
     * @brief The triangles of the object
     */
    std::vector<Triangle> triangles;

    /**
     * @brief Construct a new Object with no points or triangles
     */
    Object() = default;

    /**
     * @brief Construct a new Object from a file
     * @param file The file to read from
     * @throws std::runtime_error If the file is not in the correct format
     */
    explicit Object(std::ifstream &file);
};