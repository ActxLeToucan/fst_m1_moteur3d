#pragma once

#include <vector>

#include "Triangle.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "tgaimage.h"
#include "Options.h"

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
     * @brief The normal map of the object
     */
    TGAImage normalMap;

    /**
     * @brief Construct a new Object with no points or triangles
     */
    Object() = default;

    /**
     * @brief Construct a new Object from a file
     * @param options The rendering options
     * @param fileName The name of the file
     * @throws std::runtime_error If the file is not in the correct format
     */
    explicit Object(Options &options, std::string fileName);

    /**
     * @brief Get the points of a triangle
     * @param triangle The triangle
     * @return A tuple containing the points of the triangle in homogeneous coordinates
     */
    std::tuple<const glm::vec4, const glm::vec4, const glm::vec4> getTrianglePoints(Triangle &triangle) const;

    /**
     * @brief Get the normals of a triangle
     * @param triangle The triangle
     * @return A tuple containing the normals of the triangle
     */
    std::tuple<const glm::vec3, const glm::vec3, const glm::vec3> getTriangleNormals(Triangle &triangle) const;
};