// #include "Eigen/src/Core/Matrix.h"
#include <Eigen/Dense>
#include "assimp/mesh.h"
#include "include/Amodel.h"
#include <vector>
class mod
{
public:
    aiMesh*                 mesh;
    std::vector<aiFace>     faces;
    std::vector<aiVector3D> vertices;

    mod(const std::string modelpath)
    {
        Assimp::Importer importer;
        // 这里有个问题就是，在这个括号结束后，importer函数就会析构，内部资源就没了
        // 所以需要用其他的数组保存下所有的数据。
        const aiScene* scene = importer.ReadFile(modelpath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
        // 这里假设每个模型文件只有一个mesh
        // for (int i = 0; i < scene->mNumMeshes; i++)
        // {
        mesh = scene->mMeshes[0];
        for (int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace temp = mesh->mFaces[i];
            faces.push_back(temp);
        }
        for (int i = 0; i < mesh->mNumVertices; i++)
        {
            vertices.push_back(mesh->mVertices[i]);
        }
    }

    void test()
    {
        std::cout << "faces: " << faces.size() << std::endl;
        std::cout << "vertices: " << vertices.size() << std::endl;
    }
    Eigen::Vector3f vert(int i)
    {
        aiVector3D& v = vertices[i];
        return {v.x, v.y, v.z};
    }
};

int main()
{
    mod model(R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\Fox\glTF\Fox.gltf)");
    // model.test();
    auto temp = model.vert(1);
    std::cout << temp << std::endl;
    // model.test();
    return 0;
}