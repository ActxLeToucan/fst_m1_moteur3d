#pragma once

#include <vector>

#include "Triangle.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "tgaimage.h"

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
     * @param file The file to read from
     * @param texture The texture image
     * @param normalMap The normal map image
     * @throws std::runtime_error If the file is not in the correct format
     */
    explicit Object(std::ifstream &file, TGAImage &texture, TGAImage &normalMap);

    /**
     * @brief Get the points of a triangle
     * @param triangle The triangle
     * @return A tuple containing the points of the triangle
     */
    std::tuple<const glm::vec4, const glm::vec4, const glm::vec4> getTrianglePoints(Triangle &triangle) const;

    /**
     * @brief Get the normals of a triangle
     * @param triangle The triangle
     * @return A tuple containing the normals of the triangle
     */
    std::tuple<const glm::vec3, const glm::vec3, const glm::vec3> getTriangleNormals(Triangle &triangle) const;
};