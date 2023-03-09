#include <functional>
#include <limits>
#include <new>

#include "glm/matrix.hpp"
#include "model.h"
// #include "our_gl.h"
#include "shader/BaseShader.h"

constexpr int       width  = 800; // output image size
constexpr int       height = 800;
glm::vec3           light_dir{1, 1, 1}; // light source
glm::vec3           eye{-1, 1, 3};      // camera position
constexpr glm::vec3 center{0, 0, 0};    // camera direction
constexpr glm::vec3 up{0, 1, 0};        // camera up vector

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) { 
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

int main(int argc, char **argv)
{
    Model model(R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\diablo3_pose\diablo3_pose.obj)");
    TGAImage image(width, height, TGAImage::RGB);

    for (int i = 0; i < model.nfaces(); i++)
    {
        std::vector<glm::vec3> face(3);
        for (int j : {0,1,2}){
            face[j]= model.vert(i,j);
        }
        
        for (int j = 0; j < 3; j++)
        {
            glm::vec3 v0 = face[j];
            glm::vec3 v1 = face[(j + 1) % 3];
            int   x0 = (v0.x + 1.) * width / 2.;
            int   y0 = (v0.y + 1.) * height / 2.;
            int   x1 = (v1.x + 1.) * width / 2.;
            int   y1 = (v1.y + 1.) * height / 2.;
            line(x0, y0, x1, y1, image, TGAColor{255,255,255});
        }
    }

    image.write_tga_file("output.tga");
    return 0;
}

// int main(int argc, char **argv)
// {
//     TGAImage framebuffer(width, height, TGAImage::RGB);             // the output image
//     lookat(eye, center, up);                                        // build the ModelView matrix
//     viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4); // build the Viewport matrix
//     projection(length(eye - center));                               // build the Projection matrix
//     std::vector<double> zbuffer(width * height, std::numeric_limits<double>::max());

//     for (int m = 0; m < 1; m++)
//     { // iterate through all input objects

//         Model model(R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\diablo3_pose\diablo3_pose.obj)");

//         Shader shader(model);
//         for (int i = 0; i < model.nfaces(); i++)
//         {                           // for every triangle
//             glm::vec4 clip_vert[3]; // triangle coordinates (clip coordinates), written by VS, read by FS
//             for (int j =0;j<3;j++)
//                 shader.vertex(i, j, clip_vert[j]);             // call the vertex shader for each triangle vertex
//             triangle(clip_vert, shader, framebuffer, zbuffer); // actual rasterization routine call
//         }
//     }
//     framebuffer.write_tga_file("changes.tga");
//     return 0;
// }
