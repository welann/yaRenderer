#include "tgaimage.h"
// #include "geometry.h"
#include <glm/glm.hpp>

void viewport(const int x, const int y, const int w, const int h);
void projection(const double coeff = 0); // coeff = -1/c
void lookat(const glm::vec3 eye, const glm::vec3 center, const glm::vec3 up);

struct IShader
{
    static TGAColor sample2D(const TGAImage &img, glm::vec2 &uvf)
    {
        return img.get(uvf[0] * img.width(), uvf[1] * img.height());
    }
    virtual bool fragment(const glm::vec3 bar, TGAColor &color) = 0;
};

void triangle(glm::vec4 clip_verts[3], IShader &shader, TGAImage &image, std::vector<double> &zbuffer);
