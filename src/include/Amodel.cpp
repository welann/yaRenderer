#include "Amodel.h"
#include <iostream>

Amodel::Amodel(const std::string modelpath)
{
    Assimp::Importer importer;
    scene = importer.ReadFile(modelpath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
    // 这里假设每个模型文件只有一个mesh
    for (int i = 0; i < scene->mNumMeshes; i++)
    {
        meshes.push_back(scene->mMeshes[i]);
    }

    std::cout << "Number of meshes: " << scene->mNumMeshes << std::endl;
    std::cout << "Number of materials: " << scene->mNumMaterials << std::endl;
    std::cout << "Number of textures: " << scene->mNumTextures << std::endl;
    std::cout << "Number of lights: " << scene->mNumLights << std::endl;
    std::cout << "Number of animations: " << scene->mNumAnimations << std::endl;
    std::cout << "Number of cameras: " << scene->mNumCameras << std::endl;
    for (int i = 0; i < scene->mNumMesher; i++)
    {
        std::cout << "Number of model: " << i << " faces: " << meshes[i]->mNumFaces << std::endl;
    }
}

void Amodel::loadTexture(const std::string &filepath)
{
}

TGAImage Amodel::getTexture(const std::string &texturekind)
{
}

int Amodel::nverts()
{
}
int Amodel::nfaces()
{
}
Eigen::Vector3f Amodel::normal(int iface, int nthvert)
{
}
Eigen::Vector3f Amodel::normal(Eigen::Vector2f uv)
{
}
Eigen::Vector3f Amodel::vert(int i)
{
}
Eigen::Vector3f Amodel::vert(int iface, int nthvert)
{
}
Eigen::Vector2f Amodel::uv(int iface, int nthvert)
{
}
TGAColor Amodel::diffuse(Eigen::Vector2f uv)
{
}
float Amodel::specular(Eigen::Vector2f uv)
{
}
