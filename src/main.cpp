#include <vector>
#include <limits>
#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"

#include <Eigen/Dense>

Model *model = NULL;

const int width  = 800;
const int height = 800;

Vec3f light_dir(1, 1, 3);
Vec3f eye(2, 0, 4);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

Eigen::Vector3f Light_dir{1.0f, 1.0f, 3.0f};
Eigen::Vector3f Eye{2.0f, 0.0f, 4.0f};
Eigen::Vector3f Center{0.0f, 0.0f, 0.0f};
Eigen::Vector3f Up{0.0f, 1.0f, 0.0f};

struct Shader : public IShader
{
    mat<2, 3, float> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    mat<4, 3, float> varying_tri; // triangle coordinates (screen space), written by VS, read by FS

    Eigen::Matrix<float,4,3> varying_triMatrix; 

    virtual Vec4f vertex(int iface, int nthvert)
    {
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        Vec4f gl_Vertex = Projection * ModelView * embed<4>(model->vert(iface, nthvert));
        // std::cout<<"Vertex "<<gl_Vertex[0]<<" "<<gl_Vertex[1]<<" "<<gl_Vertex[2]<<" "<<gl_Vertex[3]<<"\n";
        //=============
        Vec3f           vert = model->vert(iface, nthvert);
        Eigen::Vector4f inflateVert;
        inflateVert << vert[0], vert[1], vert[2], 1.0f;
        Eigen::Vector4f Gl_Vertex = ProjectionMatrix * ModelViewMatrix * inflateVert;
        // std::cout<<"Evert "<<Gl_Vertex <<std::endl;
        //============
        varying_tri.set_col(nthvert, gl_Vertex);
        // varying_triMatrix.col(nthvert) = Gl_Vertex;
        varying_triMatrix<<varying_tri[0][0],varying_tri[0][1],varying_tri[0][2],
                            varying_tri[1][0],varying_tri[1][1],varying_tri[1][2],
                            varying_tri[2][0],varying_tri[2][1],varying_tri[2][2],
                            varying_tri[3][0],varying_tri[3][1],varying_tri[3][2];
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

    lookat(eye, center, up);
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    projection(-1.f / (eye - center).norm());
    light_dir = proj<3>((Projection * ModelView * embed<4>(light_dir, 0.f))).normalize();
    // std::cout<<"Light: "<<light_dir[0]<<" "<<light_dir[1]<<" "<<light_dir[2]<<std::endl;

    Eigen::Vector4f inflatelight;
    inflatelight << Light_dir, 0.0f;
    Eigen::Vector4f Light = (ProjectionMatrix * ModelViewMatrix * inflatelight);

    // 注意，这里要先赋值Light_dir后归一化Light_dir，不能Light归一化后赋值Light_dir
    Light_dir << Light(0), Light(1), Light(2);
    Light_dir.normalize();

    // std::cout<<"LightE: "<<Light_dir<<std::endl;

    for (int m = 0; m < 1; m++)
    {
        model = new Model("/workspaces/yaRenderer/models/diablo3_pose/diablo3_pose.obj");
        Shader shader;
        for (int i = 0; i < model->nfaces(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                shader.vertex(i, j);
            }
            // Eigen::Matrix<float, 4, 3> clipc2;
            // triangle2(shader.varying_triMatrix, shader, frame, zbuffer);
            triangle(shader.varying_tri, shader, frame, zbuffer);
        }
        delete model;
    }

    frame.flip_vertically(); // to place the origin in the bottom left corner of the image
    frame.write_tga_file("framebuffer.tga");

    delete[] zbuffer;
    return 0;
}
