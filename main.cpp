#include <iostream>
#include <fstream>
#include <vector>

#include "tgaimage.h"
#include "Object.h"
#include "glm/mat3x3.hpp"

#define HEIGHT 800
#define WIDTH 800

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

void drawTriangle(Triangle &triangle, Object &object, TGAImage &image, glm::vec3 &light, std::vector<float> &zBuffer) {
    glm::vec3 p1_monde = object.points[triangle.v1-1];
    glm::vec3 p2_monde = object.points[triangle.v2-1];
    glm::vec3 p3_monde = object.points[triangle.v3-1];

    // illumination
    glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(p2_monde - p1_monde), glm::vec3(p3_monde - p1_monde)));
    float intensity = glm::dot(normal, light);
    if (intensity < 0) {
        return;
    }

    // convert to screen space
    glm::ivec2 p1 = p1_monde * glm::vec3(WIDTH/2, HEIGHT/2, 1) + glm::vec3(WIDTH/2, HEIGHT/2, 0);
    glm::ivec2 p2 = p2_monde * glm::vec3(WIDTH/2, HEIGHT/2, 1) + glm::vec3(WIDTH/2, HEIGHT/2, 0);
    glm::ivec2 p3 = p3_monde * glm::vec3(WIDTH/2, HEIGHT/2, 1) + glm::vec3(WIDTH/2, HEIGHT/2, 0);

    glm::ivec2 boundingBoxMin = glm::min(glm::min(p1, p2), p3);
    glm::ivec2 boundingBoxMax = glm::max(glm::max(p1, p2), p3);

    glm::mat3x3 matInv = glm::inverse(glm::mat3x3(p1.x, p1.y, 1,
                                    p2.x, p2.y, 1,
                                    p3.x, p3.y, 1));

    // fill triangle
    for (int x = boundingBoxMin.x; x <= boundingBoxMax.x; x++) {
        for (int y = boundingBoxMin.y; y <= boundingBoxMax.y; y++) {
            glm::vec3 p(x, y, 1);
            glm::vec3 barycentric = matInv * p;

            if (glm::any(glm::lessThan(barycentric, glm::vec3(0)))) {
                continue;
            }

            // z-buffer
            p.z = glm::dot(glm::vec3(p1_monde.z, p2_monde.z, p3_monde.z), barycentric);
            if (zBuffer[x + y * WIDTH] < p.z) {
                zBuffer[x + y * WIDTH] = p.z;

                // get texture color
                glm::vec2 uv = object.textureCoords[triangle.vt1-1] * barycentric.x +
                               object.textureCoords[triangle.vt2-1] * barycentric.y +
                               object.textureCoords[triangle.vt3-1] * barycentric.z;
                const TGAColor textureColor = object.texture.get(
                    uv.x * object.texture.width(),
                    uv.y * object.texture.height());

                // set color
                TGAColor color = TGAColor({static_cast<uint8_t>(textureColor.bgra[0] * intensity),
                    static_cast<uint8_t>(textureColor.bgra[1] * intensity),
                    static_cast<uint8_t>(textureColor.bgra[2] * intensity)});
                image.set(x, y, color);
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <filename> <texture>" << std::endl;
        return 1;
    }

    // Read texture
    TGAImage texture;
    if (!texture.read_tga_file(argv[2])) {
        std::cerr << "Could not open file " << argv[2] << std::endl;
        return 1;
    }

    // Read object
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Could not open file " << argv[1] << std::endl;
        return 1;
    }
    Object object(file, texture);

    // z-buffer
    std::vector<float> zBuffer(WIDTH * HEIGHT, std::numeric_limits<float>::lowest());

    // Draw image
    glm::vec3 light(0, 0, 1);
    TGAImage image(WIDTH, HEIGHT, TGAImage::Format::RGB);
    for (Triangle &triangle : object.triangles) {
        drawTriangle(triangle, object, image, light, zBuffer);
    }
    return !image.write_tga_file("output.tga");
}
