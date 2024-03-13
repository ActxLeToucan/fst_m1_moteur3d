#include <iostream>
#include <fstream>
#include <vector>
#include "tgaimage.h"

struct Point {
    // dans [-1, 1]
    float x, y, z;
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

    // Read vertices
    std::vector<Point> points;
    std::string line;
    while (getline(file, line)) {
        if (line[0] == 'v' && line[1] == ' ') {
            Point p;
            if (sscanf(line.c_str(), "v %f %f %f", &p.x, &p.y, &p.z) != 3) {
                std::cerr << "Error reading vertex at line " << points.size() + 1 << std::endl;
                return 1;
            }
            points.push_back(p);
        }
    }
    std::cout << "Read " << points.size() << " points" << std::endl;

    // Draw image
    TGAImage image(800, 800, TGAImage::Format::RGB);
    for (Point &p : points) {
        int x = (p.x + 1) * 400;
        int y = (p.y + 1) * 400;
        image.set(x, y, TGAColor{255, 255, 255, 255});
    }
    image.write_tga_file("output.tga");
}
