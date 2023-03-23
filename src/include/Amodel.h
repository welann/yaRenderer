#include <Eigen/Dense>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

using namespace Assimp;

class Amodel
{
public:
    explicit Amodel(std::string modelpath);
    ~Amodel()= default;;

    void      loadTexture(TextureKind kind, const std::string& filepath);
    TGAImage* getTexture(TextureKind);

    size_t             nverts();
    size_t             nfaces();
    Eigen::Vector3f normal(int iface, int nthvert);
    Eigen::Vector3f normal(Eigen::Vector2f uv);
    Eigen::Vector3f vert(int i);
    Eigen::Vector3f vert(int iface, int nthvert);
    Eigen::Vector2f uv(int iface, int nthvert);
    TGAColor        diffuse(Eigen::Vector2f uv);
    float           specular(Eigen::Vector2f uv);
    // std::vector<int> face(int idx);
    void test(){
        // std::cout<<"aiscene : "<<scene<<std::endl;
        // std::cout<<"aimesh : "<<mesh<<std::endl;
    }
private:
    // aiScene* scene;
    // aiMesh*        mesh;
    material       _material;

    std::vector<aiFace> faces;
    std::vector<aiVector3D> vertices;
    std::vector<aiVector3D> normals;
    std::vector<aiVector3D> uvs;


    

};
