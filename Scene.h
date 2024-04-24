#pragma once

#include <vector>
#include "Object.h"
#include <algorithm>

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
private:
    /**
     * @brief Create an object from a line and add it to the scene
     * @param line The line to parse
     * @param folder The folder containing the scene file
     */
    void createObject(const std::string &line, const std::string &folder);

    /**
     * @brief Trim whitespace from a string
     * @param[in,out] s The string to trim
     * @return Trimmed string
     */
    static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
    }

    /**
     * @brief Trim whitespace from the left of a string
     * @copyright https://stackoverflow.com/questions/44973435/stdptr-fun-replacement-for-c17/44973498#44973498
     * @param[in,out] s The string to trim
     * @return Trimmed string
     */
    static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);}));
        return s;
    }

    /**
     * @brief Trim whitespace from the right of a string
     * @copyright https://stackoverflow.com/questions/44973435/stdptr-fun-replacement-for-c17/44973498#44973498
     * @param[in,out] s The string to trim
     * @return Trimmed string
     */
    static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c);}).base(), s.end());
        return s;
    }
};
