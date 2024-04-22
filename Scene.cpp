#include "Scene.h"
#include "glm/ext/matrix_transform.hpp"

Scene::Scene(Options options, const std::string &fileName) : options(options) {
    options.check();

    // Read scene
    std::ifstream file(fileName);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + fileName);
    }

    if (fileName.find(".obj") != std::string::npos) {
        objects.push_back(Object(options, fileName));
        return;
    }

    std::string folder = fileName.substr(0, fileName.find_last_of('/') + 1);

    std::string line;
    int lineNum = 0;
    while (getline(file, line)) {
        lineNum++;
        if (line.find("FILE=") == 0) {
            std::string objectFileName = folder + line.substr(5);
            std::cout << "Read object " << objectFileName << std::endl;
            objects.push_back(Object(options, objectFileName));
        } else if (line.find(".translate ") == 0) {
            glm::vec3 vec;
            if (sscanf(line.c_str(), ".translate %f %f %f", &vec.x, &vec.y, &vec.z) != 3) {
                throw std::runtime_error("Error reading file at line " + std::to_string(lineNum));
            }
            objects.back().modelMat = glm::translate(objects.back().modelMat, vec);
        } else if (line.find(".scale ") == 0) {
            glm::vec3 vec;
            if (sscanf(line.c_str(), ".scale %f %f %f", &vec.x, &vec.y, &vec.z) != 3) {
                throw std::runtime_error("Error reading file at line " + std::to_string(lineNum));
            }
            objects.back().modelMat = glm::scale(objects.back().modelMat, vec);
        } else if (line.find(".rotate ") == 0) {
            float degrees;
            int axis;
            if (sscanf(line.c_str(), ".rotate %i %f", &axis, &degrees) != 2) {
                throw std::runtime_error("Error reading file at line " + std::to_string(lineNum));
            }
            glm::vec3 vec(0);
            vec[axis] = 1;
            objects.back().modelMat = glm::rotate(objects.back().modelMat, glm::radians(degrees), vec);
        } else if (line.find("LIGHT ") == 0) {
            glm::vec3 pos;
            if (sscanf(line.c_str(), "LIGHT %f %f %f", &pos.x, &pos.y, &pos.z) != 3) {
                throw std::runtime_error("Error reading file at line " + std::to_string(lineNum));
            }
            light = glm::normalize(pos);
        } else if (line.find("EYE ") == 0) {
            glm::vec3 pos;
            if (sscanf(line.c_str(), "EYE %f %f %f", &pos.x, &pos.y, &pos.z) != 3) {
                throw std::runtime_error("Error reading file at line " + std::to_string(lineNum));
            }
            eye = pos;
        }
    }

    if (objects.empty()) {
        throw std::runtime_error("No objects found in file " + fileName);
    }
}
