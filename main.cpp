#include <iostream>
#include <fstream>
#include <vector>
#include "tgaimage.h"
#include "Object.h"

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

void drawTriangle(Triangle &triangle, std::vector<Point> &points, TGAImage &image, TGAColor color) {
    drawLine(points[triangle.v1-1].x*400+400, points[triangle.v1-1].y*400+400, points[triangle.v2-1].x*400+400, points[triangle.v2-1].y*400+400, image, color);
    drawLine(points[triangle.v2-1].x*400+400, points[triangle.v2-1].y*400+400, points[triangle.v3-1].x*400+400, points[triangle.v3-1].y*400+400, image, color);
    drawLine(points[triangle.v3-1].x*400+400, points[triangle.v3-1].y*400+400, points[triangle.v1-1].x*400+400, points[triangle.v1-1].y*400+400, image, color);
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
    Object object(file);

    // Draw image
    TGAImage image(800, 800, TGAImage::Format::RGB);
    for (Triangle &triangle : object.triangles) {
        drawTriangle(triangle, object.points, image, TGAColor({255, 255, 255}));
    }
    image.write_tga_file("output.tga");
}
