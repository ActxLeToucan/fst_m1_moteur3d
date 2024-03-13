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

void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    int derror2 = std::abs(dy)*2;
    int error2 = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1>y0?1:-1);
            error2 -= dx*2;
        }
    }
}

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
    drawLine(10, 10, 200, 300, image, TGAColor{255, 0, 0, 255});
    drawLine(10, 10, 200, 100, image, TGAColor{0, 255, 0, 255});
    drawLine(10, 10, 100, 200, image, TGAColor{0, 0, 255, 255});
    image.write_tga_file("output.tga");
}
