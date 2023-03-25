#include "Model.h"
#include <iostream>
#include "tgaimage.h"
#include "../external/stb_image.h"
#include "../external/stb_image_write.h"

Model::Model(std::string modelpath)
{
    Assimp::Importer importer;
    const aiScene*   scene = importer.ReadFile(modelpath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
    // 这里假设每个模型文件只有一个mesh
    // for (int i = 0; i < scene->mNumMeshes; i++)
    // {
    aiMesh* mesh = scene->mMeshes[0];
    std::cout << "mesh in" << std::endl;
    // }

    std::cout << "Number of meshes: " << scene->mNumMeshes << std::endl;
    std::cout << "Number of materials: " << scene->mNumMaterials << std::endl;
    std::cout << "Number of textures: " << scene->mNumTextures << std::endl;
    std::cout << "Number of lights: " << scene->mNumLights << std::endl;
    std::cout << "Number of cameras: " << scene->mNumCameras << std::endl;
    std::cout << "Number of animations: " << scene->mNumAnimations << std::endl;
    std::cout << "Number of faces: " << mesh->mNumFaces << std::endl;

    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        faces.push_back(mesh->mFaces[i]);
    }

    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        vertices.push_back(mesh->mVertices[i]);
        normals.push_back(mesh->mNormals[i]);
        uvs.push_back(mesh->mTextureCoords[0][i]);
    }

    const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
    // {
        aiString texturePath;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);

        // 这里在干的事情就是：
        // 先获取到贴图的具体位置，这里我先写死了为 temp
        // 随后使用stb_image库读取png文件，转换为为 tga文件
        // 随后新建一个TGAImage对象读取这个tga文件
        // 最后把这个对象复制给_material相对应的贴图里

        std::string temp     = R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\Box With Spaces\glTF\)";
        std::string fullPath = std::string(temp + texturePath.C_Str());
        int         width, height, channels;
        std::cout<<"fullPath: "<<fullPath<<std::endl;

        unsigned char* data   = stbi_load(R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\Box With Spaces\glTF\glTF Logo With Spaces.png)", &width, &height, &channels, 0);
        int            result = stbi_write_tga("frame0.tga", width, height, channels, data);

        TGAImage frame1(1024, 1024, TGAImage::RGB);
        frame1.read_tga_file("frame0.tga");

        TGAImage tempdiffuse(frame1);
        // tempdiffuse.write_tga_file("tempdiffuse.tga");
        _material.setTexture(TextureKind::diffuse, tempdiffuse);

        std::string filename = texturePath.C_Str();
        std::cout << "filename: " << filename << std::endl;
        std::cout << "fullPath: " << fullPath << std::endl;
        std::cout << "width: " << width << std::endl;
        std::cout << "height: " << height << std::endl;
        std::cout << "channels: " << channels << std::endl;
    // }
}

void Model::loadTexture(const TextureKind kind, const std::string& filepath)
{
    TGAImage img;
    img.read_tga_file(filepath.c_str());
    std::cout << "loading new texture file path: " << filepath << "\n " << (img.read_tga_file(filepath.c_str()) ? "ok" : "failed") << std::endl;

    _material.setTexture(kind, img);
}

TGAImage* Model::getTexture(TextureKind kind)
{
    return _material[kind];
}

size_t Model::nverts()
{
    return vertices.size();
}

size_t Model::nfaces()
{
    return faces.size();
}

Eigen::Vector3f Model::normal(int iface, int nthvert)
{
    aiFace     face  = faces[iface];
    int        index = face.mIndices[nthvert];
    aiVector3D n     = normals[index];
    return {n.x, n.y, n.z};
}

Eigen::Vector3f Model::vert(int i)
{
    aiVector3D v = vertices[i];
    return {v.x, v.y, v.z};
}

Eigen::Vector3f Model::vert(int iface, int nthvert)
{
    aiFace     face  = faces[iface];
    int        index = face.mIndices[nthvert];
    aiVector3D v     = vertices[index];
    return {v.x, v.y, v.z};
}

Eigen::Vector2f Model::uv(int iface, int nthvert)
{
    aiFace     face  = faces[iface];
    int        index = face.mIndices[nthvert];
    aiVector3D uv    = uvs[index];
    return {uv.x, uv.y};
}

Eigen::Vector3f Model::normal(Eigen::Vector2f uvf)
{
    TGAImage*       normal = getTexture(TextureKind::normal);
    Eigen::Vector3i temp((int)(uvf[0] * normal->get_width()), (int)(uvf[1] * normal->get_height()));
    return temp.cast<float>();
}

TGAColor Model::diffuse(Eigen::Vector2f uv)
{
    //    TGAImage*       diff = getTexture(TextureKind::diffuse);
    // std::cout << "uv: \n"
            //   << uv << std::endl;
    TGAImage*       diff = _material[TextureKind::diffuse];
    Eigen::Vector2i uvi{uv[0] * diff->get_width(), uv[1] * diff->get_height()};
    // Eigen::Vector2i uvi(uv.cast<int>());
    // std::cout << "uv: \n"
    //           << uv << std::endl;
    // std::cout << "uvi: \n"
    //           << uvi << std::endl;
    return diff->get(uvi[0], uvi[1]);
}

float Model::specular(Eigen::Vector2f uv)
{
    TGAImage*       specular = getTexture(TextureKind::diffuse);
    Eigen::Vector2i uvi{uv[0] * specular->get_width(), uv[1] * specular->get_height()};
    return specular->get(uvi[0], uvi[1])[0] / 1.0f;
}
