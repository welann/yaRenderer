#include "geometry.h"

#include "our_gl.h"

glm::mat4 ModelView;
glm::mat4 Viewport;
glm::mat4 Projection;

void viewport(const int x, const int y, const int w, const int h)
{
    Viewport = {{w / 2., 0, 0, x + w / 2.},
                {0, h / 2., 0, y + h / 2.},
                {0, 0, 1, 0},
                {0, 0, 0, 1}};
}

void projection(const double f)
{ // check https://en.wikipedia.org/wiki/Camera_matrix
    Projection = {{1, 0, 0, 0},
                  {0, -1, 0, 0},
                  {0, 0, 1, 0},
                  {0, 0, -1 / f, 0}};
}

void lookat(const glm::vec3 eye, const glm::vec3 center, const glm::vec3 up)
{ // check https://github.com/ssloy/tinyrenderer/wiki/Lesson-5-Moving-the-camera
    glm::vec3 z    = glm::normalize(center - eye);
    glm::vec3 x    = glm::normalize(glm::cross(up, z));
    glm::vec3 y    = glm::normalize(glm::cross(z, x));
    
    glm::mat4 Minv = {{x.x, x.y, x.z, 0},
                      {y.x, y.y, y.z, 0},
                      {z.x, z.y, z.z, 0},
                      {0, 0, 0, 1}};

    glm::mat4 Tr   = {{1, 0, 0, -eye.x},
                      {0, 1, 0, -eye.y},
                      {0, 0, 1, -eye.z},
                      {0, 0, 0, 1}};
    ModelView      = Minv * Tr;
}

glm::vec3 barycentric(const glm::vec2 tri[3], const glm::vec2 P)
{
    glm::mat3 ABC = {{embed<3>(tri[0]), embed<3>(tri[1]), embed<3>(tri[2])}};
    if (glm::determinant(ABC) < 1e-3)
        return {-1, 1, 1}; // for a degenerate triangle generate negative coordinates, it will be thrown away by the rasterizator

    return invert_transpose(ABC) * embed<3>(P);
}

void triangle(const glm::vec4 clip_verts[3], IShader &shader, TGAImage &image, std::vector<double> &zbuffer)
{
    glm::vec4 pts[3]  = {Viewport * clip_verts[0], Viewport * clip_verts[1], Viewport * clip_verts[2]};          // triangle screen coordinates before persp. division
    glm::vec2 pts2[3] = {proj<2>(pts[0] / pts[0][3]), proj<2>(pts[1] / pts[1][3]), proj<2>(pts[2] / pts[2][3])}; // triangle screen coordinates after  perps. division

    int bboxmin[2] = {image.width() - 1, image.height() - 1};
    int bboxmax[2] = {0, 0};
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 2; j++)
        {
            bboxmin[j] = std::min(bboxmin[j], static_cast<int>(pts2[i][j]));
            bboxmax[j] = std::max(bboxmax[j], static_cast<int>(pts2[i][j]));
        }
#pragma omp parallel for
    for (int x = std::max(bboxmin[0], 0); x <= std::min(bboxmax[0], image.width() - 1); x++)
    {
        for (int y = std::max(bboxmin[1], 0); y <= std::min(bboxmax[1], image.height() - 1); y++)
        {
            glm::vec3 bc_screen = barycentric(pts2, {static_cast<double>(x), static_cast<double>(y)});
            glm::vec3 bc_clip   = {bc_screen.x / pts[0][3], bc_screen.y / pts[1][3], bc_screen.z / pts[2][3]};
            bc_clip             = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z); // check https://github.com/ssloy/tinyrenderer/wiki/Technical-difficulties-linear-interpolation-with-perspective-deformations
            double frag_depth   = dot(glm::vec3{clip_verts[0][2], clip_verts[1][2], clip_verts[2][2]}, bc_clip);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 || frag_depth > zbuffer[x + y * image.width()]) continue;
            TGAColor color{255, 33, 22};
            if (shader.fragment(bc_clip, color)) continue; // fragment shader can discard current fragment
            zbuffer[x + y * image.width()] = frag_depth;
            image.set(x, y, color);
        }
    }
}
