// #include <glm/glm.hpp> // glm::vec3
// #include<iostream>

// #include <assimp/Importer.hpp>
// #include <assimp/postprocess.h>
// #include <assimp/scene.h>

// int main(){
//     glm::vec3 a = { 3.0, 2.0, 3.0 };
//     glm::vec3 b = { 1.0, 0.0, 3.0 };
//     glm::vec3 c = glm::normalize(a*b);
//     std::cout<<c.x<<" "<<c.y<<" "<<c.z<<std::endl;

//     Assimp::Importer Importer;
//     const aiScene* scene = Importer.ReadFile(R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\floor.obj)", aiProcess_Triangulate | aiProcess_FlipUVs);

//     if(scene){
//         std::cout<<"successfully imported"<<std::endl;
//     }

//     return 0;
// }
// // #include <iostream>

// // #include <Eigen/Dense>

// // // using namespace Eigen;
// // int main()
// // {
// //     Matrix2d a;
// //     a << 1, 2,
// //         3, 4;
// //     MatrixXd b(2, 2);
// //     b << 2, 3,
// //         1, 4;
// //     std::cout << "a + b =\n"
// //               << a + b << std::endl;
// //     std::cout << "a - b =\n"
// //               << a - b << std::endl;
// //     std::cout << "Doing a += b;" << std::endl;
// //     a += b;
// //     std::cout << "Now a =\n"
// //               << a << std::endl;
// //     Vector3d v(1, 2, 3);
// //     Vector3d w(1, 0, 0);
// //     std::cout << "-v + w - v =\n"
// //               << -v + w - v << std::endl;
// // }