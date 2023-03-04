#include <vector>
#include <limits>
#include <iostream>
#include <string>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"

Model *model = NULL;

const int width  = 800;
const int height = 800;

Vec3f light_dir(-1, 1, 1);
Vec3f eye(0, 1, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

struct Shader : public IShader
{
    mat<2, 3, float> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    mat<4, 3, float> varying_tri; // triangle coordinates (clip coordinates), written by VS, read by FS
    mat<3, 3, float> varying_nrm; // normal per vertex to be interpolated by FS
    mat<3, 3, float> ndc_tri;     // triangle in normalized device coordinates
                                  // iface: 三角形的序号 nthvert: 三角形三个顶点的序号
    virtual Vec4f vertex(int iface, int nthvert)
    { // 对于这个set_col，它会进行这样的运算：
        // 首先对于每个三角形，有三个顶点a(0,1,0) b(1,0,0) c(0,0,1)
        // 由于每次调用vertex函数只处理一个点
        // set_col会涉及到一个row矩阵，矩阵的大小是 vec<3,float> rows[2]
        // 所以从a点开始，假设a点对应的uv坐标为[3,4],会把row矩阵设置成
        //[0,0,3]
        //[0,0,4]
        // 一直到填充完
        // 需要注意的是，这个row矩阵是复用的，只有第0个才会被0初始化，剩下的都是覆盖前一个计算出来的结果
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        // if(iface==0){
        //     varying_uv.printm();
        //     auto v=model->uv(iface, nthvert);
        //     std::cout<<v[0]<<" "<<v[1]<<std::endl;
        // }
        varying_nrm.set_col(nthvert, proj<3>((Projection * ModelView).invert_transpose() * embed<4>(model->normal(iface, nthvert), 0.f)));
        Vec4f gl_Vertex = Projection * ModelView * embed<4>(model->vert(iface, nthvert));
        varying_tri.set_col(nthvert, gl_Vertex);
        ndc_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color)
    {
        // 这部分是计算法线
        // 将原本的切空间中的法线转换到模型坐标中
        Vec3f bn = (varying_nrm * bar).normalize();
        Vec2f uv = varying_uv * bar;

        mat<3, 3, float> A;
        A[0] = ndc_tri.col(1) - ndc_tri.col(0);
        A[1] = ndc_tri.col(2) - ndc_tri.col(0);
        A[2] = bn;

        mat<3, 3, float> AI = A.invert();

        Vec3f i = AI * Vec3f(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
        Vec3f j = AI * Vec3f(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);

        mat<3, 3, float> B;
        B.set_col(0, i.normalize());
        B.set_col(1, j.normalize());
        B.set_col(2, bn);

        Vec3f n = (B * model->normal(uv)).normalize();
        // 计算出来后就用来处理颜色
        float diff = std::max(0.f, n * light_dir);
        color      = model->diffuse(uv) * diff;

        return false;
    }
};

int main(int argc, char **argv)
{
    // mat<3,3,float> a;
    // a[0]={1,0,0};
    // a[1]={1,2,1};
    // a[2]={2,2,2};
    // a.printm();
    // auto b=a.invert();
    // b.printm();
    // auto c=a.transpose();
    // c.printm();

    float *zbuffer = new float[width * height];
    for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max())
        ;

    TGAImage frame(width, height, TGAImage::RGB);

    lookat(eye, center, up);
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    projection(-1.f / (eye - center).norm());
    light_dir = proj<3>((Projection * ModelView * embed<4>(light_dir, 0.f))).normalize();

    std::string Objaddress[2] = {
        R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\african_head\african_head_eye_inner.obj)",
        R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\african_head\african_head.obj)"

    };
    for (int i = 0; i < 2; i++)
    {
        model = new Model((char *)Objaddress[i].c_str());

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
