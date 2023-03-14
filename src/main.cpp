#include <limits>
#include <iostream>
#include "tgaimage.h"

#include "Render.h"

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

int main(int argc, char **argv)
{
    float *zbuffer      = new float[width * height];
    float *shadowbuffer = new float[width * height];

    for (int i = width * height; --i;)
    {
        zbuffer[i] = shadowbuffer[i] = -std::numeric_limits<float>::max();
    }

    TGAImage frame(width, height, TGAImage::RGB);

    lookat(Eye,Center,Up);
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    projection(-1.f / (eye - center).norm());

    // light_dir = proj<3>((Projection * ModelView * embed<4>(light_dir, 0.f))).normalize();

    Eigen::Vector4f inflatelight;
    inflatelight << Light_dir, 0.0f;
    Eigen::Vector4f Light = (ProjectionMatrix * ModelViewMatrix * inflatelight);

    // 注意，这里要先赋值Light_dir后归一化Light_dir，不能Light归一化后赋值Light_dir
    Light_dir << Light(0), Light(1), Light(2);
    Light_dir.normalize();

    for (int m = 0; m < 1; m++)
    {
        model = new Model(R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\floor.obj)");
        Shader shader;
        Render render(model, shader, &frame, zbuffer);
        render.Rendering();

        delete model;
    }

    frame.flip_vertically(); // to place the origin in the bottom left corner of the image
    frame.write_tga_file("cramebuffer.tga");

    delete[] zbuffer;
    return 0;
}
