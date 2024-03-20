#include <iostream>
#include <fstream>
#include <vector>
#include "tgaimage.h"
#include "Object.h"
#include "glm/common.hpp"
#include "glm/vec2.hpp"
#include "glm/mat3x3.hpp"

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

void drawTriangle(Triangle &triangle, std::vector<glm::vec3> &points, TGAImage &image, TGAColor color) {
    glm::ivec3 p1 = points[triangle.v1-1] * 400.f + 400.f;
    glm::ivec3 p2 = points[triangle.v2-1] * 400.f + 400.f;
    glm::ivec3 p3 = points[triangle.v3-1] * 400.f + 400.f;

    glm::ivec3 boundingBoxMin = glm::min(glm::min(p1, p2), p3);
    glm::ivec3 boundingBoxMax = glm::max(glm::max(p1, p2), p3);

    glm::mat3x3 matInv = glm::inverse(glm::mat3x3(p1.x, p1.y, 1,
                                    p2.x, p2.y, 1,
                                    p3.x, p3.y, 1));

    //draw bounding box
//    drawLine(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMax.x, boundingBoxMin.y, image, color);
//    drawLine(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMax.x, boundingBoxMax.y, image, color);
//    drawLine(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMin.x, boundingBoxMax.y, image, color);
//    drawLine(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMin.x, boundingBoxMin.y, image, color);

    // fill triangle
    for (int x = boundingBoxMin.x; x <= boundingBoxMax.x; x++) {
        for (int y = boundingBoxMin.y; y <= boundingBoxMax.y; y++) {
            glm::ivec3 p(x, y, 1);
            glm::vec3 barycentric = matInv * p;

            if (glm::all(glm::greaterThanEqual(barycentric, glm::vec3(0)))) {
                image.set(x, y, color);
            }
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
    Object object(file);

    // Draw image
    TGAImage image(800, 800, TGAImage::Format::RGB);
    for (Triangle &triangle : object.triangles) {
        TGAColor randomColor = TGAColor({static_cast<uint8_t>(std::rand() % 255), static_cast<uint8_t>(std::rand() % 255), static_cast<uint8_t>(std::rand() % 255)});
        drawTriangle(triangle, object.points, image, randomColor);
    }
    image.write_tga_file("output.tga");
}
