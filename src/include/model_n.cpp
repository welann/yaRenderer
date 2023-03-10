#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include "model_n.h"

using namespace ya;

Model::Model(const std::string filename)
{
    // Importer object
    Assimp::Importer importer;

    // Read the file
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenNormals);

    // Check for errors
    // assert(!(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode));
    std::cout << "Number of meshes: " << scene->mNumMeshes << std::endl;
    std::cout << "Number of materials: " << scene->mNumMaterials << std::endl;
    std::cout << "Number of textures: " << scene->mNumTextures << std::endl;
    std::cout << "Number of lights: " << scene->mNumLights << std::endl;
    std::cout << "Number of animations: " << scene->mNumAnimations << std::endl;
    std::cout << "Number of cameras: " << scene->mNumCameras << std::endl;

    // Get the mesh
    aiMesh* mesh = scene->mMeshes[0];

    std::cout << "Number of faces: " << mesh->mNumFaces << std::endl;

    // 获得每个面对应的点，似乎有点多余？
    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < 3; j++)
        {
            int index = face.mIndices[j];

            aiVector3D& v=mesh->mVertices[index];
            verts.push_back({v.x, v.y, v.z});
            
            aiVector3D& n = mesh->mNormals[index];
            norms.push_back({n.x,n.y,n.z});

            aiVector3D& uv = mesh->mTextureCoords[0][index];
            tex_coord.push_back({uv.x,uv.y});




        }
    }
}

int Model::nverts() const
{
    return verts.size();
}

int Model::nfaces() const
{
    return facet_vrt.size() / 3;
}

glm::vec3 Model::vert(const int i) const
{
    return verts[i];
}

glm::vec3 Model::vert(const int iface, const int nthvert) const
{
    return verts[facet_vrt[iface * 3 + nthvert]];
}

glm::vec2 Model::uv(const int iface, const int nthvert) const
{
    return tex_coord[facet_tex[iface * 3 + nthvert]];
}

glm::vec3 Model::normal(const int iface, const int nthvert) const
{
    return norms[facet_nrm[iface * 3 + nthvert]];
}
