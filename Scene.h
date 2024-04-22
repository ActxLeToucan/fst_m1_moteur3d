#pragma once

#include <vector>
#include "Object.h"

class Scene {
public:
    /**
     * @brief Options for rendering
     */
    Options options;
    /**
     * @brief The objects in the scene
     */
    std::vector<Object> objects;
    /**
     * @brief The light position
     */
    glm::vec3 light = glm::normalize(glm::vec3(1, 1, 1));
    /**
     * @brief The eye position
     */
    glm::vec3 eye = glm::vec3(0, 0, 2);

    explicit Scene(class Options options, const std::string &fileName);
};
