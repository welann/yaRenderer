#include <limits>
#include <iostream>
#include "tgaimage.h"

#include "Render.h"

#include <Eigen/Dense>

Model *amodel = nullptr;

const int width  = 1024;
const int height = 1024;

Eigen::Vector3f Light_dir{1.0f, 1.0f, 3.0f};
Eigen::Vector3f Eye{4.0f, 3.0f, 16.0f};
Eigen::Vector3f Center{0.0f, 0.0f, 0.0f};
Eigen::Vector3f Up{0.0f, 1.0f, 0.0f};

int main()
{
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);
    float *shadowbuffer = new float[width * height];

    // for (int i = width * height; --i;)
    // {
    //     zbuffer[i] = shadowbuffer[i] = -std::numeric_limits<float>::max();
    // }

    TGAImage frame(width, height, TGAImage::RGB);

    lookat(Eye, Center, Up);
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    projection(-1.f / (Eye - Center).norm());

    Eigen::Vector4f inflatelight;
    inflatelight << Light_dir, 0.0f;
    Eigen::Vector4f Light = (ProjectionMatrix * ModelViewMatrix * inflatelight);

    // 注意，这里要先赋值Light_dir后归一化Light_dir，不能Light归一化后赋值Light_dir
    Light_dir << Light(0), Light(1), Light(2);
    Light_dir.normalize();

    for (int m = 0; m < 1; m++)
    {
        amodel = new Model(R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\Box With Spaces\glTF\Box With Spaces.gltf)");


        Shader shader;
        Render render(amodel, shader, &frame, &zbuffer);
        render.Rendering();

        delete amodel;
    }
    
    zbuffer.write_tga_file("zbuffer.tga");

    frame.flip_vertically(); // to place the origin in the bottom left corner of the image
    frame.write_tga_file("Fox.tga");

    // delete[] zbuffer;
    return 0;
}
