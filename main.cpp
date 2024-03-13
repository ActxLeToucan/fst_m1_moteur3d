#include <iostream>
#include <fstream>
#include <vector>
#include "tgaimage.h"

struct Point {
    // dans [-1, 1]
    float x, y, z;
};

struct Triangle {
    // id des points (commence à 1)
    int v1, v2, v3;
    // id des textures (commence à 1)
    int vt1, vt2, vt3;
    // id des normales (commence à 1)
    int vn1, vn2, vn3;
};

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Could not open file " << argv[1] << std::endl;
        return 1;
    }

    // Read object
    std::vector<Point> points;
    std::vector<Triangle> triangles;
    std::string line;
    while (getline(file, line)) {
        if (line[0] == 'v' && line[1] == ' ') {
            // Read vertices
            Point p;
            if (sscanf(line.c_str(), "v %f %f %f", &p.x, &p.y, &p.z) != 3) {
                std::cerr << "Error reading vertex at line " << points.size() + 1 << std::endl;
                return 1;
            }
            points.push_back(p);
        } else if (line[0] == 'f' && line[1] == ' ') {
            // Read faces
            Triangle t;
            if (sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &t.v1, &t.vt1, &t.vn1, &t.v2, &t.vt2, &t.vn2, &t.v3, &t.vt3, &t.vn3) != 9) {
                std::cerr << "Error reading face at line " << points.size() + 1 << std::endl;
                return 1;
            }
            triangles.push_back(t);
        }
    }
    std::cout << "Read " << points.size() << " points" << std::endl;
    std::cout << "Read " << triangles.size() << " triangles" << std::endl;

    // Draw image
    TGAImage image(800, 800, TGAImage::Format::RGB);
    for (Point &p : points) {
        int x = (p.x + 1) * 400;
        int y = (p.y + 1) * 400;
        image.set(x, y, TGAColor{255, 255, 255, 255});
    }
    image.write_tga_file("output.tga");
}
