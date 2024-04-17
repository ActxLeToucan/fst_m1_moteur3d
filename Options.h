#pragma once

#include <iostream>

class Options {
public:
    bool wireframe = false;
    bool zBuffer = false;
    bool illumination = true;
    bool diffuse = true;
    bool perspective = true;
    bool gouraud = true;
    bool mvp = true;
    bool normalMap = true;
    bool shadows = true;

    explicit Options(int argc, char **argv) {
        bool unknown = false;
        for (int i = 2; i < argc; i++) {
            if (argv[i][0] == '-' && argv[i][2] == ':' && (argv[i][3] == '0' || argv[i][3] == '1')) {
                switch (argv[i][1]) {
                    case 'w':
                        wireframe = argv[i][3] == '1';
                        break;
                    case 'z':
                        zBuffer = argv[i][3] == '1';
                        break;
                    case 'i':
                        illumination = argv[i][3] != '0';
                        break;
                    case 'd':
                        diffuse = argv[i][3] != '0';
                        break;
                    case 'p':
                        perspective = argv[i][3] != '0';
                        break;
                    case 'g':
                        gouraud = argv[i][3] != '0';
                        break;
                    case 'm':
                        mvp = argv[i][3] != '0';
                        break;
                    case 'n':
                        normalMap = argv[i][3] != '0';
                        break;
                    case 's':
                        shadows = argv[i][3] != '0';
                        break;
                    default:
                        std::cerr << "Unknown option: " << argv[i] << std::endl;
                        unknown = true;
                        break;
                }
            } else {
                std::cerr << "Unknown option: " << argv[i] << std::endl;
                unknown = true;
            }
        }
        if (unknown) {
            std::cerr << std::endl;
            usage(stderr);
            exit(1);
        }
    }

    void check() {
        if (wireframe) {
            zBuffer = false;
            illumination = false;
            diffuse = false;
            gouraud = false;
            normalMap = false;
            shadows = false;
        } else if (zBuffer) {
            illumination = false;
            diffuse = false;
            gouraud = false;
            normalMap = false;
            shadows = false;
        } else if (!illumination) {
            gouraud = false;
            normalMap = false;
            shadows = false;
        }
        std::cout << "Options:" << std::endl;
        std::cout << "  Wireframe: " << wireframe << std::endl;
        std::cout << "  Z-Buffer: " << zBuffer << std::endl;
        std::cout << "  Illumination: " << illumination << std::endl;
        std::cout << "  Diffuse texture: " << diffuse << std::endl;
        std::cout << "  Perspective projection: " << perspective << std::endl;
        std::cout << "  Gouraud shading: " << gouraud << std::endl;
        std::cout << "  Model view projection: " << mvp << std::endl;
        std::cout << "  Normal mapping: " << normalMap << std::endl;
        std::cout << "  Shadows: " << shadows << std::endl;
    }

    inline void disableDiffuse() {
        diffuse = false;
        std::cout << "Disabling diffuse texture" << std::endl;
    }

    inline void disableNormalMap() {
        normalMap = false;
        std::cout << "Disabling normal map" << std::endl;
    }

    static void usage(FILE *out = stdout) {
        fprintf(out, "Options:\n");
        fprintf(out, "  -w:[0|1] Wireframe mode\t\tdefault: 0\n");
        fprintf(out, "  -z:[0|1] Z-Buffer mode\t\tdefault: 0\n");
        fprintf(out, "  -i:[0|1] Illumination\t\t\tdefault: 1\n");
        fprintf(out, "  -d:[0|1] Diffuse texture\t\tdefault: 1\n");
        fprintf(out, "  -p:[0|1] Perspective projection\tdefault: 1\n");
        fprintf(out, "  -g:[0|1] Gouraud shading\t\tdefault: 1\n");
        fprintf(out, "  -m:[0|1] Model view projection\tdefault: 1\n");
        fprintf(out, "  -n:[0|1] Normal mapping\t\tdefault: 1\n");
        fprintf(out, "  -s:[0|1] Shadows\t\t\tdefault: 1\n");
        fprintf(out, "\n0 = disabled, 1 = enabled\n");
    }
};