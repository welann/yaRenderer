#include <vector>
#include <limits>
#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"

Model *model = NULL;

const int width  = 800;
const int height = 800;

Vec3f light_dir(1, 1, 3);
Vec3f eye(2, 0, 4);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

// struct DepthShader : public IShader
// {
//     mat<3, 3, float> varying_tri;

//     DepthShader() : varying_tri() {}

//     virtual Vec4f vertex(int iface, int nthvert)
//     {
//         Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));         // read the vertex from .obj file
//         gl_Vertex       = Viewport * Projection * ModelView * gl_Vertex; // transform it to screen coordinates
//         varying_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
//         return gl_Vertex;
//     }

//     virtual bool fragment(Vec3f bar, TGAColor &color)
//     {
//         Vec3f p = varying_tri * bar;
//         color   = TGAColor(255, 255, 255) * (p.z / depth);
//         return false;
//     }
// };

struct Shader : public IShader
{
    mat<2, 3, float> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    mat<4, 3, float> varying_tri; // triangle coordinates (screen space), written by VS, read by FS

    virtual Vec4f vertex(int iface, int nthvert)
    {
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        Vec4f gl_Vertex = Projection * ModelView * embed<4>(model->vert(iface, nthvert));
        varying_tri.set_col(nthvert, gl_Vertex);
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color)
    {
        Vec2f uv = varying_uv * bar;
        color    = model->diffuse(uv);
        return false;
    }
};

int main(int argc, char **argv)
{
    float *zbuffer      = new float[width * height];
    float *shadowbuffer = new float[width * height];

    for (int i = width * height; --i;)
    {
        zbuffer[i] = shadowbuffer[i] = -std::numeric_limits<float>::max();
    }

    TGAImage frame(width, height, TGAImage::RGB);

    // model = new Model(R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\diablo3_pose\diablo3_pose.obj)");
    // light_dir.normalize();

    // { // rendering the shadow buffer
    //     TGAImage depth(width, height, TGAImage::RGB);
    //     lookat(light_dir, center, up);
    //     viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    //     projection(0);

    //     DepthShader      depthshader;
    //     mat<4, 3, float> screen_coords;
    //     for (int i = 0; i < model->nfaces(); i++)
    //     {
    //         for (int j = 0; j < 3; j++)
    //         {
    //             screen_coords[0][j] = depthshader.vertex(i, j)[0];
    //             screen_coords[1][j] = depthshader.vertex(i, j)[1];
    //             screen_coords[2][j] = depthshader.vertex(i, j)[2];
    //             screen_coords[3][j] = depthshader.vertex(i, j)[3];
    //         }
    //         triangle(screen_coords, depthshader, depth, shadowbuffer);
    //     }
    //     depth.flip_vertically(); // to place the origin in the bottom left corner of the image
    //     depth.write_tga_file("depth.tga");
    // }

    lookat(eye, center, up);
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    projection(-1.f / (eye - center).norm());
    light_dir = proj<3>((Projection * ModelView * embed<4>(light_dir, 0.f))).normalize();

    for (int m = 0; m < 1; m++)
    {
        model = new Model(R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\diablo3_pose\diablo3_pose.obj)");
        Shader shader;
        for (int i = 0; i < model->nfaces(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                shader.vertex(i, j);
            }
            triangle(shader.varying_tri, shader, frame, zbuffer);
        }
        delete model;
    }
    frame.flip_vertically(); // to place the origin in the bottom left corner of the image
    frame.write_tga_file("framebuffer.tga");

    delete[] zbuffer;
    return 0;
}
