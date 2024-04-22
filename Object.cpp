#include "Object.h"
#include <fstream>
#include <iostream>

Object::Object(const Options &options, std::string fileName) {
    // Read object
    std::ifstream file(fileName);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + fileName);
    }

    std::string line;
    int lineNum = 0;
    while (getline(file, line)) {
        lineNum++;
        if (line[0] == 'v' && line[1] == ' ') {
            // Read vertices
            glm::vec3 p;
            if (sscanf(line.c_str(), "v %f %f %f", &p.x, &p.y, &p.z) != 3) {
                throw std::runtime_error("Error reading file at line " + std::to_string(lineNum));
            }
            points.push_back(p);
        } else if (line[0] == 'v' && line[1] == 't') {
            // Read texture coordinates
            glm::vec3 t;
            int res = sscanf(line.c_str(), "vt %f %f %f", &t.x, &t.y, &t.z);
            if (res != 2 && res != 3) {
                throw std::runtime_error("Error reading file at line " + std::to_string(lineNum));
            }
            if (res == 2) {
                t.z = 0;
            }
            t.y = 1 - t.y; // Invert y-axis
            textureCoords.push_back(t);
        } else if (line[0] == 'v' && line[1] == 'n') {
            // Read normals
            glm::vec3 n;
            if (sscanf(line.c_str(), "vn %f %f %f", &n.x, &n.y, &n.z) != 3) {
                throw std::runtime_error("Error reading file at line " + std::to_string(lineNum));
            }
            normals.push_back(n);
        } else if (line[0] == 'f' && line[1] == ' ') {
            // Read faces
            Triangle t{};
            if (sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &t.v1, &t.vt1, &t.vn1, &t.v2, &t.vt2, &t.vn2,
                       &t.v3, &t.vt3, &t.vn3) != 9) {
                throw std::runtime_error("Error reading file at line " + std::to_string(lineNum));
            }
            triangles.push_back(t);
        }
    }
    std::cout << "Read " << points.size() << " points" << std::endl;
    std::cout << "Read " << triangles.size() << " triangles" << std::endl;


    fileName = fileName.substr(0, fileName.find_last_of('.'));

    // Read texture
    if (options.diffuse) {
        std::string textureFilename = fileName + "_diffuse.tga";
        texture.second = texture.first.read_tga_file(textureFilename);
    }

    // Read normal map
    if (options.normalMap) {
        std::string normalMapFilename = fileName + "_nm_tangent.tga";
        normalMap.second = normalMap.first.read_tga_file(normalMapFilename);
    }

    // Read specular map
    if (options.specular) {
        std::string specularFilename = fileName + "_spec.tga";
        specularMap.second = specularMap.first.read_tga_file(specularFilename);
    }
}

std::tuple<const glm::vec4, const glm::vec4, const glm::vec4> Object::getTrianglePoints(Triangle &triangle) const {
    return {
            glm::vec4(points[triangle.v1 - 1], 1),
            glm::vec4(points[triangle.v2 - 1], 1),
            glm::vec4(points[triangle.v3 - 1], 1)
    };
}

std::tuple<const glm::vec3, const glm::vec3, const glm::vec3> Object::getTriangleNormals(Triangle &triangle) const {
    return {
            glm::normalize(glm::vec3(glm::transpose(glm::inverse(modelMat)) * glm::vec4(normals[triangle.vn1 - 1], 0))),
            glm::normalize(glm::vec3(glm::transpose(glm::inverse(modelMat)) * glm::vec4(normals[triangle.vn2 - 1], 0))),
            glm::normalize(glm::vec3(glm::transpose(glm::inverse(modelMat)) * glm::vec4(normals[triangle.vn3 - 1], 0)))
    };
}
