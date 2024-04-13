#include "Object.h"
#include <fstream>
#include <iostream>

Object::Object(std::string fileName) {
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
            if (sscanf(line.c_str(), "vt %f %f %f", &t.x, &t.y, &t.z) != 3) {
                throw std::runtime_error("Error reading file at line " + std::to_string(lineNum));
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
            if (sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &t.v1, &t.vt1, &t.vn1, &t.v2, &t.vt2, &t.vn2, &t.v3, &t.vt3, &t.vn3) != 9) {
                throw std::runtime_error("Error reading file at line " + std::to_string(lineNum));
            }
            triangles.push_back(t);
        }
    }
    std::cout << "Read " << points.size() << " points" << std::endl;
    std::cout << "Read " << triangles.size() << " triangles" << std::endl;


    fileName = fileName.substr(0, fileName.find_last_of('.'));

    // Read texture
    std::string textureFilename = fileName + "_diffuse.tga";
    if (!texture.read_tga_file(textureFilename)) {
        throw std::runtime_error("Could not open file " + textureFilename);
    }

    // Read normal map
    std::string normalMapFilename = fileName + "_nm_tangent.tga";
    if (!normalMap.read_tga_file(normalMapFilename)) {
        throw std::runtime_error("Could not open file " + normalMapFilename);
    }
}

std::tuple<const glm::vec4, const glm::vec4, const glm::vec4> Object::getTrianglePoints(Triangle &triangle) const {
    return {
      glm::vec4(points[triangle.v1-1], 1),
      glm::vec4(points[triangle.v2-1], 1),
      glm::vec4(points[triangle.v3-1], 1)
    };
}

std::tuple<const glm::vec3, const glm::vec3, const glm::vec3> Object::getTriangleNormals(Triangle &triangle) const {
    return {
      normals[triangle.vn1-1],
      normals[triangle.vn2-1],
      normals[triangle.vn3-1]
    };
}
