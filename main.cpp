#include <iostream>
#include <fstream>
#include <vector>

#include "Object.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "tgaimage.h"

#define HEIGHT 4096
#define WIDTH 4096

#define USE_LIGHT 1

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

void drawTriangle(Triangle &triangle, Object &object, TGAImage &image, glm::vec3 &light, float* zBuffer, glm::mat4 &mvp) {
    // local homogeneous coordinates
    auto [p1_local, p2_local, p3_local] = object.getTrianglePoints(triangle);

    // apply model view projection
    glm::vec4 p1_mvp = mvp * p1_local;
    glm::vec4 p2_mvp = mvp * p2_local;
    glm::vec4 p3_mvp = mvp * p3_local;

    glm::vec3 p1 = glm::vec3(p1_mvp) / p1_mvp.w;
    glm::vec3 p2 = glm::vec3(p2_mvp) / p2_mvp.w;
    glm::vec3 p3 = glm::vec3(p3_mvp) / p3_mvp.w;

    // convert to screen space
    glm::ivec2 p1_screen = p1 * glm::vec3(WIDTH/2, HEIGHT/2, 1) + glm::vec3(WIDTH/2, HEIGHT/2, 0);
    glm::ivec2 p2_screen = p2 * glm::vec3(WIDTH/2, HEIGHT/2, 1) + glm::vec3(WIDTH/2, HEIGHT/2, 0);
    glm::ivec2 p3_screen = p3 * glm::vec3(WIDTH/2, HEIGHT/2, 1) + glm::vec3(WIDTH/2, HEIGHT/2, 0);


    // fill triangle
    glm::ivec2 boundingBoxMin = glm::min(glm::min(p1_screen, p2_screen), p3_screen);
    glm::ivec2 boundingBoxMax = glm::max(glm::max(p1_screen, p2_screen), p3_screen);

    glm::mat3x3 matInv = glm::inverse(glm::mat3x3(p1_screen.x, p1_screen.y, 1,
                                                  p2_screen.x, p2_screen.y, 1,
                                                  p3_screen.x, p3_screen.y, 1));
    auto [normal1, normal2, normal3] = object.getTriangleNormals(triangle);
    for (int x = boundingBoxMin.x; x < boundingBoxMax.x; x++) {
        for (int y = boundingBoxMin.y; y < boundingBoxMax.y; y++) {
            glm::vec3 p(x, y, 1);
            glm::vec3 barycentric = matInv * p;

            // a point can be outside the screen, in that case we skip it
            if (p.x + p.y * WIDTH < 0 || p.x + p.y * WIDTH >= WIDTH * HEIGHT) {
                continue;
            }

            if (glm::any(glm::lessThan(barycentric, glm::vec3(-0.01)))) {
                continue;
            }

            // illumination
            glm::vec3 normal = normal1 * barycentric.x + normal2 * barycentric.y + normal3 * barycentric.z;
            float intensity = USE_LIGHT ? glm::dot(normal, light) : 1;
            if (intensity < 0) {
                continue;
            }

            // z-buffer
            p.z = glm::dot(glm::vec3(p1.z, p2.z, p3.z), barycentric);
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
    auto zBuffer = new float[WIDTH * HEIGHT];
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        zBuffer[i] = std::numeric_limits<float>::lowest();
    }

    /* Model view projection */
    // model matrix
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(1.5));
    glm::mat4 translate = glm::translate(scale, glm::vec3(0, 0, -1));
    glm::mat4 modelMat = glm::rotate(translate, glm::radians(20.f), glm::vec3(0, 1, 0));

    // view matrix
    glm::mat4x4 viewMat = glm::lookAt(
        glm::vec3(0, 0, 1), // camera position in world space
        glm::vec3(0, 0, 0), // point to look at
        glm::vec3(0, 1, 0));

    // projection matrix
    const float distanceToCamera = 3;
    glm::mat4 projectionMat = glm::mat4 {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, -1.f / distanceToCamera,
        0, 0, 0, 1
    };

    glm::mat4 mvp = projectionMat * viewMat * modelMat;

    // Draw image
    glm::vec3 light = glm::normalize(glm::vec3(0, 0, 1));
    TGAImage image(WIDTH, HEIGHT, TGAImage::Format::RGB);
    for (Triangle &triangle : object.triangles) {
        drawTriangle(triangle, object, image, light, zBuffer, mvp);
    }

    delete[] zBuffer;

    return !image.write_tga_file("output.tga");
}
