#include <functional>
#include <limits>
#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "model.h"
// #include "our_gl.h"
#include "shader/BaseShader.h"
#include "external/stb_image.h"
#include "model_n.h"

using namespace std;

constexpr int width  = 800; // output image size
constexpr int height = 800;

glm::vec3 light_dir{1, 1, 1}; // light source
glm::vec3 eye{-1, 1, 3};      // camera position

constexpr glm::vec3 center{0, 0, 0};   // camera direction
constexpr glm::vec3 upVector{0, 1, 0}; // camera up vector

glm::vec3 cameraPosition{1, 1, 3};
glm::vec3 cameraTarget{0, 0, 0};

glm::mat4 projectionMatrix = glm::perspective(
    glm::radians(45.0f),          // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90&deg; (extra wide) and 30&deg; (quite zoomed in)
    (float)width / (float)height, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
    0.1f,                         // Near clipping plane. Keep as big as possible, or you'll get precision issues.
    100.0f                        // Far clipping plane. Keep as little as possible.
);

glm::mat4 ModelMatrix = glm::mat4(1.0f);

glm::mat4 CameraMatrix = glm::lookAt(
    cameraPosition, // the position of your camera, in world space
    cameraTarget,   // where you want to look at, in world space
    upVector        // probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
);

glm::mat4 MVPmatrix = projectionMatrix * CameraMatrix * ModelMatrix;

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx      = x1 - x0;
    int dy      = y1 - y0;
    int derror2 = std::abs(dy) * 2;
    int error2  = 0;
    int y       = y0;
    for (int x = x0; x <= x1; x++)
    {
        if (steep)
        {
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx)
        {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

// int main(int argc, char** argv)
// {
//     // Model    model(R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\diablo3_pose\diablo3_pose.obj)");
//     // TGAImage image(width, height, TGAImage::RGB);

//     // for (int i = 0; i < model.nfaces(); i++)
//     // {
//     //     std::vector<glm::vec3> face(3);
//     //     for (int j : {0, 1, 2})
//     //     {
//     //         face[j] = model.vert(i, j);
//     //     }

//     //     for (int j = 0; j < 3; j++)
//     //     {
//     //         glm::vec3 v0 = face[j];
//     //         // cout << v0.x << " " < < v0.y << " " << v0.z << std::endl;
//     //         glm::vec3 v1 = face[(j + 1) % 3];
//     //         int       x0 = (v0.x + 1.) * width / 2.;
//     //         int       y0 = (v0.y + 1.) * height / 2.;
//     //         int       x1 = (v1.x + 1.) * width / 2.;
//     //         int       y1 = (v1.y + 1.) * height / 2.;
//     //         line(x0, y0, x1, y1, image, TGAColor{255, 255, 255});
//     //     }
//     // }

//     Assimp::Importer importer;
//     const aiScene*   scene = importer.ReadFile(R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\diablo3_pose\diablo3_pose.obj)", aiProcess_Triangulate | aiProcess_FlipUVs);

//     TGAImage image(width, height, TGAImage::RGB);

//     aiMesh* mesh = scene->mMeshes[0];

//     vector<glm::vec3> vert;

//     // cout<<mesh->mVertices[1][0]<<" "<<mesh->mVertices[1][1]<<" "<<mesh->mVertices[1][2]<<" "<<mesh->mVertices[1][3];
//     for (int i = 0; i < mesh->mNumFaces; i++)
//     {
//         const aiFace& face = mesh->mFaces[i];
//         for (unsigned int vertex_idx = 0; vertex_idx < 3; vertex_idx++)
//         {
//             const aiVector3D& v0 = mesh->mVertices[face.mIndices[vertex_idx]];
//             const aiVector3D& v1 = mesh->mVertices[face.mIndices[(vertex_idx + 1) % 3]];
//             int               x0 = (v0.x + 1.0f) * 400; // 坐标映射到图片大小中
//             int               y0 = (v0.y + 1.0f) * 400;
//             int               x1 = (v1.x + 1.0f) * 400;
//             int               y1 = (v1.y + 1.0f) * 400;
//             line(x0, y0, x1, y1, image, TGAColor{255,255,255,255});
//         }  
//     }
//     image.write_tga_file("output1.tga");
//     return 0;
// }
    int main(int argc, char **argv)
    {
        TGAImage framebuffer(width, height, TGAImage::RGB);             // the output image
        lookat(eye, center, upVector);                                        // build the ModelView matrix
        viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4); // build the Viewport matrix
        projection(length(eye - center));                               // build the Projection matrix
        std::vector<double> zbuffer(width * height, std::numeric_limits<double>::max());
        Assimp::Importer importer;
        const aiScene*   scene = importer.ReadFile(R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\diablo3_pose\diablo3_pose.obj)", aiProcess_Triangulate | aiProcess_FlipUVs);
        for (int m = 0; m < scene->mNumMeshes; m++)
        {  
            const aiMesh* mesh = scene->mMeshes[m];
            Shader shader(model);

            for (int i = 0; i < mesh->mNumFaces; i++)
            {                         
                const aiFace& face = mesh->mFaces[i];
                glm::vec4 clip_vert[3]; // triangle coordinates (clip coordinates), written by VS, read by FS
                for (int j =0;j<3;j++)
                    shader.vertex(i, j, clip_vert[j]);             // call the vertex shader for each triangle vertex
                triangle(clip_vert, shader, framebuffer, zbuffer); // actual rasterization routine call
            }
        }
        framebuffer.write_tga_file("changes.tga");
        return 0;
    }
