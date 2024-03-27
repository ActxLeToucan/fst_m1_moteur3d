#include <iostream>
#include <fstream>
#include "Object.h"

Object::Object(std::ifstream &file, TGAImage &texture) : texture(texture) {
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
}
