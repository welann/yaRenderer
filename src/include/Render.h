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
    Model    *model = nullptr;
    Shader    shader;
    TGAImage *image;
    float    *zbuffer;

public:
    void triangle(Eigen::Matrix<float, 4, 3> &clipc, IShader &shader, TGAImage &image, float *zbuffer);

    Render(Model *m, Shader &sh, TGAImage *im, float *zb) : model(m), shader(sh), image(im), zbuffer(zb) {}
    void Rendering()
    {
        for (int i = 0; i < model->nfaces(); i++)
        {
            //进行背面剔除操作
            //获取每个面的法线，如果法线方向和视线方向夹角小于90°，则舍去这个面
            //如何获得平面的法线？
            //使用叉乘的方法，对三角形两条边进行叉乘得到的就是，注意方向
            bool Isdiacard=shader.discard_face(i);
            if(Isdiacard) continue;

            for (int j = 0; j < 3; j++)
            {
                shader.vertex(i, j);
            }
            std::cout << "face: " << i << std::endl;
            triangle(shader.varying_triMatrix, shader, *image, zbuffer);
        }
    }
};