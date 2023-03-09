#include <functional>
#include <limits>
#include <new>

#include "glm/matrix.hpp"
#include "model.h"
// #include "our_gl.h"
#include"shader/BaseShader.h"

constexpr int       width  = 800; // output image size
constexpr int       height = 800;
glm::vec3 light_dir{1, 1, 1}; // light source
glm::vec3 eye{-1, 1, 3};       // camera position
constexpr glm::vec3 center{0, 0, 0};    // camera direction
constexpr glm::vec3 up{0, 1, 0};        // camera up vector


int main(int argc, char **argv)
{

    TGAImage framebuffer(width, height, TGAImage::RGB);             // the output image
    lookat(eye, center, up);                                        // build the ModelView matrix
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4); // build the Viewport matrix
    projection(length(eye - center));                               // build the Projection matrix
    std::vector<double> zbuffer(width * height, std::numeric_limits<double>::max());

    for (int m = 0; m < 1; m++)
    { // iterate through all input objects

        Model model(R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\diablo3_pose\diablo3_pose.obj)");

        Shader shader(model);
        for (int i = 0; i < model.nfaces(); i++)
        {                           // for every triangle
            glm::vec4 clip_vert[3]; // triangle coordinates (clip coordinates), written by VS, read by FS
            for (int j : {0, 1, 2})
                shader.vertex(i, j, clip_vert[j]);             // call the vertex shader for each triangle vertex
            triangle(clip_vert, shader, framebuffer, zbuffer); // actual rasterization routine call
        }
    }
    framebuffer.write_tga_file("changes.tga");
    return 0;
}
