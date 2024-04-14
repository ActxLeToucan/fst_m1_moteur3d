#include <iostream>

#include "Object.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "tgaimage.h"
#include "Options.h"

#define HEIGHT 4096
#define WIDTH 4096

void drawZBuffer(TGAImage &image, float *zBuffer) {
    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::lowest();
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (zBuffer[i] == std::numeric_limits<float>::lowest()) {
            continue;
        }
        min = std::min(min, zBuffer[i]);
        max = std::max(max, zBuffer[i]);
    }
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (zBuffer[i] == std::numeric_limits<float>::lowest()) {
            continue;
        }
        int x = i % WIDTH;
        int y = i / WIDTH;
        uint8_t color = (zBuffer[i] - min) / (max - min) * 255;
        image.set(x, y, TGAColor({color, color, color}));
    }
}

void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

void drawTriangle(const Options &options, Triangle &triangle, const Object &object, TGAImage &image,
                  const glm::vec3 &light, float *zBuffer, const glm::mat4 &mvp, const glm::mat4 &modelMat) {
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
    glm::ivec2 p1_screen = p1 * glm::vec3(WIDTH / 2, HEIGHT / 2, 1) + glm::vec3(WIDTH / 2, HEIGHT / 2, 0);
    glm::ivec2 p2_screen = p2 * glm::vec3(WIDTH / 2, HEIGHT / 2, 1) + glm::vec3(WIDTH / 2, HEIGHT / 2, 0);
    glm::ivec2 p3_screen = p3 * glm::vec3(WIDTH / 2, HEIGHT / 2, 1) + glm::vec3(WIDTH / 2, HEIGHT / 2, 0);

    if (options.wireframe) {
        // draw triangle
        TGAColor color = TGAColor({255, 255, 255});
        drawLine(p1_screen.x, p1_screen.y, p2_screen.x, p2_screen.y, image, color);
        drawLine(p2_screen.x, p2_screen.y, p3_screen.x, p3_screen.y, image, color);
        drawLine(p3_screen.x, p3_screen.y, p1_screen.x, p1_screen.y, image, color);
        return;
    }

    // fill triangle
    glm::ivec2 boundingBoxMin = glm::min(glm::min(p1_screen, p2_screen), p3_screen);
    glm::ivec2 boundingBoxMax = glm::max(glm::max(p1_screen, p2_screen), p3_screen);

    glm::mat3x3 matInv = glm::inverse(glm::mat3x3(p1_screen.x, p1_screen.y, 1,
                                                  p2_screen.x, p2_screen.y, 1,
                                                  p3_screen.x, p3_screen.y, 1));
    auto [normal1, normal2, normal3] = object.getTriangleNormals(triangle, modelMat);
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

            // texture coordinates
            glm::vec2 uv = object.textureCoords[triangle.vt1 - 1] * barycentric.x +
                           object.textureCoords[triangle.vt2 - 1] * barycentric.y +
                           object.textureCoords[triangle.vt3 - 1] * barycentric.z;

            // Gouraud shading
            glm::vec3 normal = options.gouraud ? normal1 * barycentric.x + normal2 * barycentric.y +
                                                 normal3 * barycentric.z
                                               : glm::normalize(glm::cross(glm::vec3(p2 - p1), glm::vec3(p3 - p1)));

            // normal mapping
            if (options.normalMap) {
                const TGAColor normalMapColor = object.normalMap.get(uv.x * object.normalMap.width(),
                                                                     uv.y * object.normalMap.height());
                glm::vec3 normalMapColorVec(normalMapColor.bgra[2],
                                            normalMapColor.bgra[1],
                                            normalMapColor.bgra[0]);
                normalMapColorVec = normalMapColorVec / 255.f * 2.f - 1.f; // from [0, 255] to [-1, 1]
                normal = glm::normalize(normal + normalMapColorVec);
            }

            // illumination
            float intensity = options.illumination ? glm::dot(normal, light) : 1;
            if (intensity < 0) {
                intensity = 0;
            }

            // z-buffer
            p.z = glm::dot(glm::vec3(p1.z, p2.z, p3.z), barycentric);
            if (zBuffer[x + y * WIDTH] >= p.z) {
                continue;
            }
            zBuffer[x + y * WIDTH] = p.z;

            if (options.zBuffer) {
                continue;
            }

            // get texture color
            const TGAColor textureColor = options.diffuse ?
                                          object.texture.get(uv.x * object.texture.width(),
                                                             uv.y * object.texture.height()) :
                                          TGAColor({255, 255, 255});

            // set color
            TGAColor color = TGAColor({static_cast<uint8_t>(textureColor.bgra[0] * intensity),
                                       static_cast<uint8_t>(textureColor.bgra[1] * intensity),
                                       static_cast<uint8_t>(textureColor.bgra[2] * intensity)});
            image.set(x, y, color);
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename> [options]" << std::endl;
        Options::usage(stderr);
        return 1;
    }

    Options options(argc, argv);
    options.check();

    Object object(options, argv[1]);

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
//    modelMat = glm::mat4(1);
//    modelMat = glm::rotate(modelMat, glm::radians(180.f), glm::vec3(0, 1, 0));

    // view matrix
    glm::vec3 camera(0, 0, 2);
    glm::vec3 center(0, 0, 0);
    glm::mat4x4 viewMat = glm::lookAt(
            camera, // camera position in world space
            center, // point to look at
            glm::vec3(0, 1, 0));

    // projection matrix
    const float distanceToCamera = 3;
    glm::mat4 projectionMat{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, -1.f / distanceToCamera,
            0, 0, 0, 1
    };
    if (!options.perspective) projectionMat = glm::mat4(1);

    glm::mat4 mvp = projectionMat * viewMat * modelMat;
    if (!options.mvp) mvp = glm::mat4(1);

    // Draw image
    glm::vec3 light = glm::normalize(glm::vec3(0, 1, 1) - center);
    TGAImage image(WIDTH, HEIGHT, TGAImage::Format::RGB);
    for (Triangle &triangle: object.triangles) {
        drawTriangle(options, triangle, object, image, light, zBuffer, mvp, modelMat);
    }

    if (options.zBuffer) {
        drawZBuffer(image, zBuffer);
    }

    delete[] zBuffer;

    return !image.write_tga_file("output.tga");
}
