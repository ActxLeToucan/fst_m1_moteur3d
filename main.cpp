#include <iostream>
#include <fstream>
#include <vector>

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
}
