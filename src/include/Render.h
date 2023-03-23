// 在这里定义整个渲染流程
// 在main函数里准备好各种矩阵后，进行渲染
// 接收的信息：
// 模型
// shader
//

#include "../shader/Baseshader.h"


void viewport(int x, int y, int w, int h);
void projection(float coeff = 0.f); // coeff = -1/c
void lookat(Eigen::Vector3f eye, Eigen::Vector3f center, Eigen::Vector3f up);

class Render
{
private:
    Amodel    *model = nullptr;
    Shader   &shader;
    TGAImage *image;
    float    *zbuffer;


public:
    void triangle(Eigen::Matrix<float, 4, 3> &clipc, IShader &shader, TGAImage &image, float *zbuffer);

    Render(Amodel *m, Shader &sh, TGAImage *im, float *zb) : model(m), shader(sh), image(im), zbuffer(zb) {}
    void Rendering()
    {
        for (int i = 0; i < model->nfaces(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                shader.vertex(i, j);
            }
            std::cout<<"face: "<<i<<std::endl;
            triangle(shader.varying_triMatrix, shader, *image, zbuffer);
        }
    }
};