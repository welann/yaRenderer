#include "geometry.h"
#include <glm/glm.hpp>
#include "glm/ext/matrix_transform.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <iostream>
#include "our_gl.h"

#include <Eigen/Dense>

using namespace std;

glm::mat4 ModelView;
glm::mat4 Viewport;
glm::mat4 Projection;

void viewport(const int x, const int y, const int w, const int h)
{
    Viewport = {{w / 2., 0, 0, x + w / 2.},
                {0, h / 2., 0, y + h / 2.},
                {0, 0, 1, 0},
                {0, 0, 0, 1}};
    cout << "Viewport  ===============" << endl;
    cout << Viewport[0][0] << " " << Viewport[0][1] << " " << Viewport[0][2] << " " << Viewport[0][3] << endl;
    cout << Viewport[1][0] << " " << Viewport[1][1] << " " << Viewport[1][2] << " " << Viewport[1][3] << endl;
    cout << Viewport[2][0] << " " << Viewport[2][1] << " " << Viewport[2][2] << " " << Viewport[2][3] << endl;
    cout << Viewport[3][0] << " " << Viewport[3][1] << " " << Viewport[3][2] << " " << Viewport[3][3] << endl;
    cout << "                                " << endl;
}

// void projection(const double f)
// { // check https://en.wikipedia.org/wiki/Camera_matrix
//   // Projection = {{1, 0, 0, 0},
//   //               {0, -1, 0, 0},
//   //               {0, 0, 1, 0},
//   //               {0, 0, -1 / f, 0}};
//     Projection = glm::perspective(
//         glm::radians(45.0f), // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90&deg; (extra wide) and 30&deg; (quite zoomed in)
//         1.0f,                // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
//         0.1f,                // Near clipping plane. Keep as big as possible, or you'll get precision issues.
//         100.0f               // Far clipping plane. Keep as little as possible.
//     );
// }

void projection(const double f)
{ // check https://en.wikipedia.org/wiki/Camera_matrix
    Projection = {
        {1, 0, 0, 0},
        {0, -1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, -1 / f, 0}};

    cout << "Projection  ===============" << endl;
    cout << Projection[0][0] << " " << Projection[0][1] << " " << Projection[0][2] << " " << Projection[0][3] << endl;
    cout << Projection[1][0] << " " << Projection[1][1] << " " << Projection[1][2] << " " << Projection[1][3] << endl;
    cout << Projection[2][0] << " " << Projection[2][1] << " " << Projection[2][2] << " " << Projection[2][3] << endl;
    cout << Projection[3][0] << " " << Projection[3][1] << " " << Projection[3][2] << " " << Projection[3][3] << endl;
    cout << "                                " << endl;
}

// glm::mat4 projection2(const double f)
// { // check https://en.wikipedia.org/wiki/Camera_matrix
//     return {
//         {1, 0, 0, 0},
//         {0, -1, 0, 0},
//         {0, 0, 1, 0},
//         {0, 0, -1 / f, 0}};
// }

void lookat(const glm::vec3 eye, const glm::vec3 center, const glm::vec3 up)
{ // check https://github.com/ssloy/tinyrenderer/wiki/Lesson-5-Moving-the-camera
    Eigen::Vector3d t1{(center - eye).x, (center - eye).y, (center - eye).z};
    Eigen::Vector3d upv{up.x, up.y, up.z};

    Eigen::Vector3d z1 = t1.normalized();
    Eigen::Vector3d x1 = (upv.cross(z1)).normalized();
    Eigen::Vector3d y1 = (z1.cross(x1)).normalized();
    Eigen::Matrix4d lhs, rhs;

    lhs << x1(0), x1(1), x1(2), 0,
        y1(0), y1(1), y1(2), 0,
        z1(0), z1(1), z1(2), 0,
        0, 0, 0, 1;

    rhs << 1, 0, 0, -eye.x,
        0, 1, 0, -eye.y,
        0, 0, 1, -eye.z,
        0, 0, 0, 1;

    // glm::vec3 z    = glm::normalize(center - eye);
    // glm::vec3 x    = glm::normalize(glm::cross(z, up));
    // glm::vec3 y    = glm::normalize(glm::cross(x, z));
    // glm::mat4 Minv = {{x.x, x.y, x.z, 0}, {y.x, y.y, y.z, 0}, {z.x, z.y, z.z, 0}, {0, 0, 0, 1}};
    // glm::mat4 Tr   = {{1, 0, 0, -eye.x}, {0, 1, 0, -eye.y}, {0, 0, 1, -eye.z}, {0, 0, 0, 1}};

    // lhs << x.x, x.y, x.z, 0,
    //     y.x, y.y, y.z, 0,
    //     z.x, z.y, z.z, 0,
    //     0, 0, 0, 1;
    // rhs << 1, 0, 0, -eye.x,
    //     0, 1, 0, -eye.y,
    //     0, 0, 1, -eye.z,
    //     0, 0, 0, 1;

    Eigen::Matrix4d ans = lhs * rhs;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            ModelView[i][j] = ans(i, j);
        }
    }

    cout << "ModelView  ===============" << endl;
    cout << ModelView[0][0] << " " << ModelView[0][1] << " " << ModelView[0][2] << " " << ModelView[0][3] << endl;
    cout << ModelView[1][0] << " " << ModelView[1][1] << " " << ModelView[1][2] << " " << ModelView[1][3] << endl;
    cout << ModelView[2][0] << " " << ModelView[2][1] << " " << ModelView[2][2] << " " << ModelView[2][3] << endl;
    cout << ModelView[3][0] << " " << ModelView[3][1] << " " << ModelView[3][2] << " " << ModelView[3][3] << endl;
    cout << "                                " << endl;

    // ModelView = glm::lookAt(
    //     eye,    // the position of your camera, in world space
    //     center, // where you want to look at, in world space
    //     up      // probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
    // );
}

glm::mat4 lookat2(const glm::vec3 eye, const glm::vec3 center, const glm::vec3 up)
{ // check https://github.com/ssloy/tinyrenderer/wiki/Lesson-5-Moving-the-camera
    glm::vec3       z    = glm::normalize(center - eye);
    glm::vec3       x    = glm::normalize(glm::cross(up, z));
    glm::vec3       y    = glm::normalize(glm::cross(z, x));
    glm::mat4       Minv = {{x.x, x.y, x.z, 0}, {y.x, y.y, y.z, 0}, {z.x, z.y, z.z, 0}, {0, 0, 0, 1}};
    glm::mat4       Tr   = {{1, 0, 0, -eye.x}, {0, 1, 0, -eye.y}, {0, 0, 1, -eye.z}, {0, 0, 0, 1}};
    Eigen::Matrix4d lhs, rhs;
    lhs << x.x, x.y, x.z, 0,
        y.x, y.y, y.z, 0,
        z.x, z.y, z.z, 0,
        0, 0, 0, 1;
    rhs << 1, 0, 0, -eye.x,
        0, 1, 0, -eye.y,
        0, 0, 1, -eye.z,
        0, 0, 0, 1;

    Eigen::Matrix4d ans = lhs * rhs;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            ModelView[i][j] = ans(i, j);
        }
    }

    // ModelView      = glm::dot(Minv,Tr);
    cout << "Minv: =====================" << endl;
    cout << Minv[0][0] << ", " << Minv[0][1] << ", " << Minv[0][2] << ", " << Minv[0][3] << endl;
    cout << Minv[1][0] << ", " << Minv[1][1] << ", " << Minv[1][2] << ", " << Minv[1][3] << endl;
    cout << Minv[2][0] << ", " << Minv[2][1] << ", " << Minv[2][2] << ", " << Minv[2][3] << endl;
    cout << Minv[3][0] << ", " << Minv[3][1] << ", " << Minv[3][2] << ", " << Minv[3][3] << endl;

    cout << "Tr: =====================" << endl;
    cout << Tr[0][0] << ", " << Tr[0][1] << ", " << Tr[0][2] << ", " << Tr[0][3] << endl;
    cout << Tr[1][0] << ", " << Tr[1][1] << ", " << Tr[1][2] << ", " << Tr[1][3] << endl;
    cout << Tr[2][0] << ", " << Tr[2][1] << ", " << Tr[2][2] << ", " << Tr[2][3] << endl;
    cout << Tr[3][0] << ", " << Tr[3][1] << ", " << Tr[3][2] << ", " << Tr[3][3] << endl;
    cout << "=====================" << endl;

    cout << "ModelView  ===============" << endl;
    cout << ModelView[0][0] << " " << ModelView[0][1] << " " << ModelView[0][2] << " " << ModelView[0][3] << endl;
    cout << ModelView[1][0] << " " << ModelView[1][1] << " " << ModelView[1][2] << " " << ModelView[1][3] << endl;
    cout << ModelView[2][0] << " " << ModelView[2][1] << " " << ModelView[2][2] << " " << ModelView[2][3] << endl;
    cout << ModelView[3][0] << " " << ModelView[3][1] << " " << ModelView[3][2] << " " << ModelView[3][3] << endl;
    return ModelView;
}

glm::vec3 barycentric(const glm::vec2 tri[3], const glm::vec2 P)
{
    // glm::mat3 ABC = {{embed<3>(tri[0]), embed<3>(tri[1]), embed<3>(tri[2])}};
    // if (glm::determinant(ABC) < 1e-3)
    //     return {-1, 1, 1}; // for a degenerate triangle generate negative coordinates, it will be thrown away by the rasterizator
    // return invert_transpose(ABC) * embed<3>(P);

    // Compute barycentric coordinates (u, v, w) for
    // point p with respect to triangle (a, b, c)
    // 检测退化的三角形
    float epsilon = 0.000001f; // Can tweak this value depending on needs
    if (glm::abs(glm::cross(glm::vec3(tri[1] - tri[0], 0), glm::vec3(tri[2] - tri[0], 0)).z) < epsilon)
    {
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
    glm::vec3 uv{1, 1, 1};

    glm::vec2 v0    = tri[1] - tri[0];
    glm::vec2 v1    = tri[2] - tri[0];
    glm::vec2 v2    = P - tri[0];
    float     d00   = glm::dot(v0, v0);
    float     d01   = glm::dot(v0, v1);
    float     d11   = glm::dot(v1, v1);
    float     d20   = glm::dot(v2, v0);
    float     d21   = glm::dot(v2, v1);
    float     denom = d00 * d11 - d01 * d01;
    uv.y            = (d11 * d20 - d01 * d21) / denom;
    float w         = (d00 * d21 - d01 * d20) / denom;
    uv.x            = 1.0f - uv.y - w;
    // but there we only need (x,y) and set z to 1 to show that it is a coordinate
    return uv;
}

void triangle(glm::vec4 clip_verts[3], IShader &shader, TGAImage &image, std::vector<double> &zbuffer)
{
    // glm::vec3 upVector{0.0f, 1.0f, 0.0f}; // camera up vector
    // glm::vec3 cameraPosition{1.0f, 1.0f, 3.0f};
    // glm::vec3 cameraTarget{0.0f, 0.0f, 0.0f};

    // glm::mat4 projectionMatrix = glm::perspective(
    //     glm::radians(45.0f),     // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90&deg; (extra wide) and 30&deg; (quite zoomed in)
    //     (float)800 / (float)800, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
    //     0.1f,                    // Near clipping plane. Keep as big as possible, or you'll get precision issues.
    //     100.0f                   // Far clipping plane. Keep as little as possible.
    // );

    // glm::mat4 ModelMatrix = glm::mat4(1.0f);

    // // glm::mat4 CameraMatrix = glm::lookAt(
    // //     cameraPosition, // the position of your camera, in world space
    // //     cameraTarget,   // where you want to look at, in world space
    // //     upVector        // probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
    // // );
    // glm::mat4 CameraMatrix = lookat2(cameraPosition, cameraTarget, upVector);
    // // glm::mat4 MVPmatrix = projectionMatrix * CameraMatrix * ModelMatrix;

    // cout << "CameraMatrix" << endl;
    // cout << CameraMatrix[0][0] << " " << CameraMatrix[0][1] << " " << CameraMatrix[0][2] << " " << CameraMatrix[0][3] << endl;
    // cout << CameraMatrix[1][0] << " " << CameraMatrix[1][1] << " " << CameraMatrix[1][2] << " " << CameraMatrix[1][3] << endl;
    // cout << CameraMatrix[2][0] << " " << CameraMatrix[2][1] << " " << CameraMatrix[2][2] << " " << CameraMatrix[2][3] << endl;
    // cout << CameraMatrix[3][0] << " " << CameraMatrix[3][1] << " " << CameraMatrix[3][2] << " " << CameraMatrix[3][3] << endl;
    // cout << "--------------------------------" << endl;
    // glm::mat4 Projection = glm::perspective(glm::radians(45.0), 1.0, 1.0, 1000.0);
    // Projection[1][1] *= -1; // invert Y axis
    // float f=(cameraPosition-cameraTarget).length();
    // Projection[3][2] = -1.0 / f; // set near clipping plane

    // glm::mat4 Projection = projection2((cameraPosition - cameraTarget).length());
    // for (int i = 0; i < 3; i++)
    // {
    //     clip_verts[i] = CameraMatrix * clip_verts[i];
    //     std::cout << "after modelvoew: " << clip_verts[i][0] << " " << clip_verts[i][1] << " " << clip_verts[i][2] << " " << clip_verts[i][3] << std::endl;
    //     ;

    //     clip_verts[i] = Projection * clip_verts[i];
    //     std::cout << "after Projection: " << clip_verts[i][0] << " " << clip_verts[i][1] << " " << clip_verts[i][2] << " " << clip_verts[i][3] << std::endl;
    //     ;
    // }

    // for (int i = 0; i < 3; i++)
    // {
    //     clip_verts[i] = Projection * clip_verts[i];
    //     std::cout<<"after Projection: " <<clip_verts[i][0]<<" "<<clip_verts[i][1]<<" "<<clip_verts[i][2]<<" "<<clip_verts[i][2]<<" "<<clip_verts[i][3]<<std::endl;;
    // }

    ///==========================
    // glm::vec4 pts[3]  = {Viewport * clip_verts[0], Viewport * clip_verts[1], Viewport * clip_verts[2]};          // triangle screen coordinates before persp. division
    // glm::vec2 pts2[3] = {proj<2>(pts[0] / pts[0][3]), proj<2>(pts[1] / pts[1][3]), proj<2>(pts[2] / pts[2][3])}; // triangle screen coordinates after  perps. division
    // glm::vec2 pts2[3]={proj<2>(clip_verts[0]/clip_verts[0][2]),proj<2>(clip_verts[1]/clip_verts[1][2]),proj<2>(clip_verts[2]/clip_verts[2][2])};
    int bboxmin[2] = {image.width() - 1, image.height() - 1};
    int bboxmax[2] = {0, 0};

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            // bboxmin[j] = std::min(bboxmin[j], static_cast<int>(pts2[i][j]));
            // bboxmax[j] = std::max(bboxmax[j], static_cast<int>(pts2[i][j]));
            bboxmin[j] = std::min(bboxmin[j], static_cast<int>(clip_verts[i][j]));
            bboxmax[j] = std::max(bboxmax[j], static_cast<int>(clip_verts[i][j]));
        }
    }
    std::cout << "bboxmin: " << bboxmin[0] << " " << bboxmin[1] << std::endl;
    std::cout << "bboxmax: " << bboxmax[0] << " " << bboxmax[1] << std::endl;

#pragma omp parallel for
    for (int x = std::max(bboxmin[0], 0); x <= std::min(bboxmax[0], image.width() - 1); x++)
    {
        for (int y = std::max(bboxmin[1], 0); y <= std::min(bboxmax[1], image.height() - 1); y++)
        {
            // glm::vec3 bc_screen = barycentric(pts2, {static_cast<double>(x), static_cast<double>(y)});
            // glm::vec3 bc_clip   = {bc_screen.x / pts[0][3], bc_screen.y / pts[1][3], bc_screen.z / pts[2][3]};
            // bc_clip             = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z); // check https://github.com/ssloy/tinyrenderer/wiki/Technical-difficulties-linear-interpolation-with-perspective-deformations
            // double frag_depth   = dot(glm::vec3{clip_verts[0][2], clip_verts[1][2], clip_verts[2][2]}, bc_clip);
            // if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 || frag_depth > zbuffer[x + y * image.width()]) continue;
            TGAColor color{255, 33, 22};
            // if (shader.fragment(bc_clip, color)) continue; // fragment shader can discard current fragment
            // zbuffer[x + y * image.width()] = frag_depth;
            image.set(x, y, color);
            std::cout << "x: " << x << " y: " << y << std::endl;
        }
    }
}
