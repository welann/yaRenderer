#include <Eigen/Dense>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include<vector>

using namespace Assimp;




#include "Texture.h"

class Amodel
{
public:
    Amodel(const std::string modelpath);
    ~Amodel();

    void             loadTexture(const std::string &filepath);
    TGAImage         getTexture(const std::string &texturekind);

    int              nverts();
    int              nfaces();
    Eigen::Vector3f            normal(int iface, int nthvert);
    Eigen::Vector3f            normal(Eigen::Vector2f uv);
    Eigen::Vector3f            vert(int i);
    Eigen::Vector3f            vert(int iface, int nthvert);
    Eigen::Vector2f            uv(int iface, int nthvert);
    TGAColor         diffuse(Eigen::Vector2f uv);
    float            specular(Eigen::Vector2f uv);
    // std::vector<int> face(int idx);

private:
    const aiScene* scene ;
    std::vector<aiMesh* > meshes;
    material _material;
};
