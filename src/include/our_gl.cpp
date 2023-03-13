#include <cmath>
#include <limits>
#include <cstdlib>
#include "geometry.h"
#include "our_gl.h"
#include <Eigen/Dense>
#include <iostream>
using namespace std;

// using namespace Eigen;

Matrix ModelView;
Matrix Viewport;
Matrix Projection;

Eigen::Matrix4f ModelViewMatrix;
Eigen::Matrix4f ViewportMatrix;
Eigen::Matrix4f ProjectionMatrix;

IShader::~IShader() {}

void viewport(int x, int y, int w, int h)
{
    // Viewport       = Matrix::identity();
    // Viewport[0][3] = x + w / 2.f;
    // Viewport[1][3] = y + h / 2.f;
    // Viewport[2][3] = 1.f;
    // Viewport[0][0] = w / 2.f;
    // Viewport[1][1] = h / 2.f;
    // Viewport[2][2] = 0;


    ViewportMatrix << w / 2.0f, 0, 0, x + w / 2.f,
        0, h / 2.0f, 0, y + h / 2.f,
        0, 0, 0, 1.f,
        0, 0, 0, 1;

}

void projection(float coeff)
{
    Projection       = Matrix::identity();
    Projection[3][2] = coeff;

    ProjectionMatrix.setIdentity(4, 4);
    ProjectionMatrix(3, 2) = coeff;

}

void lookat(Vec3f eye, Vec3f center, Vec3f up)
{
    Vec3f z   = (eye - center).normalize();
    Vec3f x   = cross(up, z).normalize();
    Vec3f y   = cross(z, x).normalize();
    ModelView = Matrix::identity();
    for (int i = 0; i < 3; i++)
    {
        ModelView[0][i] = x[i];
        ModelView[1][i] = y[i];
        ModelView[2][i] = z[i];
        ModelView[i][3] = -center[i];
    }

    Eigen::Vector3f eye1(eye.x, eye.y, eye.z);
    Eigen::Vector3f center1(center.x, center.y, center.z);
    Eigen::Vector3f up1(up.x, up.y, up.z);

    Eigen::Vector3f z1 = (eye1 - center1).normalized();
    Eigen::Vector3f x1 = (up1.cross(z1)).normalized();
    Eigen::Vector3f y1 = (z1.cross(x1)).normalized();

    ModelViewMatrix.setIdentity(4, 4);

    for (int i = 0; i < 3; i++)
    {
        ModelViewMatrix(0, i) = x1(i);
        ModelViewMatrix(1, i) = y1(i);
        ModelViewMatrix(2, i) = z1(i);
        ModelViewMatrix(i, 3) = -center1(i);
    }
}

Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P)
{
    Eigen::Vector3f A1(A.x, A.y, 1);
    Eigen::Vector3f B1(B.x, B.y, 1);
    Eigen::Vector3f C1(C.x, C.y, 1);
    Eigen::Vector3f P1(P.x, P.y, 1);

    Eigen::Vector3f v0 = B1 - A1;
    Eigen::Vector3f v1 = C1 - A1;
    Eigen::Vector3f v2 = P1 - A1;

    float d00   = v0.dot(v0);
    float d01   = v0.dot(v1);
    float d11   = v1.dot(v1);
    float d20   = v2.dot(v0);
    float d21   = v2.dot(v1);
    float denom = d00 * d11 - d01 * d01;

    float v  = (d11 * d20 - d01 * d21) / denom;
    float w  = (d00 * d21 - d01 * d20) / denom;
    float uu = 1.0f - v - w;

    Vec3f s[2];
    for (int i = 2; i--;)
    {
        s[i][0] = C[i] - A[i];
        s[i][1] = B[i] - A[i];
        s[i][2] = A[i] - P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u[2]) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
    // return Vec3f(u,v,w);
    return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

Eigen::Vector3f barycentric2(Eigen::Vector2f A, Eigen::Vector2f B, Eigen::Vector2f C, Eigen::Vector2i P)
{
    Eigen::Vector3f A1(A[0], A[1], 1);
    Eigen::Vector3f B1(B[0], B[1], 1);
    Eigen::Vector3f C1(C[0], C[1], 1);
    Eigen::Vector3f P1(P[0], P[1], 1);

    Eigen::Vector3f v0 = B1 - A1;
    Eigen::Vector3f v1 = C1 - A1;
    Eigen::Vector3f v2 = P1 - A1;

    float d00   = v0.dot(v0);
    float d01   = v0.dot(v1);
    float d11   = v1.dot(v1);
    float d20   = v2.dot(v0);
    float d21   = v2.dot(v1);
    float denom = d00 * d11 - d01 * d01;

    float           v = (d11 * d20 - d01 * d21) / denom;
    float           w = (d00 * d21 - d01 * d20) / denom;
    float           u = 1.0f - v - w;
    Eigen::Vector3f ans;
    ans << u, v, w;
    return ans;
}

void triangle(mat<4, 3, float> &clipc, IShader &shader, TGAImage &image, float *zbuffer)
{
    mat<3, 4, float> pts = (Viewport * clipc).transpose(); // transposed to ease access to each of the points
    mat<3, 2, float> pts2;
    for (int i = 0; i < 3; i++) pts2[i] = proj<2>(pts[i] / pts[i][3]);

    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts2[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts2[i][j]));
        }
    }
    Vec2i    P;
    TGAColor color(255, 255, 255);
    int cou=0;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
    {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
        {
            Vec3f bc_screen  = barycentric(pts2[0], pts2[1], pts2[2], P);
            Vec3f bc_clip    = Vec3f(bc_screen.x / pts[0][3], bc_screen.y / pts[1][3], bc_screen.z / pts[2][3]);
            // cout<<"before norm: "<<bc_clip<<endl;
            //单位向量化的公式，要注意这不是一般的单位化，是1-范数归一化
            bc_clip          = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);
            
            float frag_depth = clipc[2] * bc_clip;

            // if(cou<10){
            //     cout<<"============="<<endl;
            //     cout<<"depth: "<<frag_depth<<endl;
            //     cout<<"bc_clip: "<<bc_clip<<endl;
            //     cout<<"cou: "<<cou<<endl;
            //     cou++;
            // }

            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 || zbuffer[P.x + P.y * image.get_width()] > frag_depth) continue;
            TGAColor color(205.0 * frag_depth, 205 * frag_depth, 205 * frag_depth);
            bool     discard = shader.fragment(bc_clip, color);
            if (!discard)
            {
                zbuffer[P.x + P.y * image.get_width()] = frag_depth;
                image.set(P.x, P.y, color);
            }
        }
    }
}

void triangle2(Eigen::Matrix<float, 4, 3> &clipc, IShader &shader, TGAImage &image, float *zbuffer)
{
    Eigen::Matrix<float, 3, 4> pts = (ViewportMatrix * clipc).transpose();
    Eigen::Matrix<float, 3, 2> pts2;
    for (int i = 0; i < 3; i++)
    {
        Eigen::Vector2f temp;
        temp << pts(i, 0) / pts(i, 3), pts(i, 1) / pts(i, 3);
        pts2(i, 0) = temp[0];
        pts2(i, 1) = temp[1];
        // pts2(i, 1) << temp[1];
    }
    Eigen::Vector2f bboxmin, bboxmax;
    bboxmin << std::numeric_limits<float>::max(), std::numeric_limits<float>::max();
    bboxmax << -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max();
    Eigen::Vector2f clamp(image.get_width() - 1, image.get_height() - 1);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts2(i, j)));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts2(i, j)));
        }
    }

    Eigen::Vector2i P;
    TGAColor        color(255, 255, 255);
    int cou=0;
    for (P[0] = bboxmin[0]; P[0] <= bboxmax[0]; P[0]++)
    {
        for (P[1] = bboxmin[1]; P[1] <= bboxmax[1]; P[1]++)
        {
            Eigen::Vector3f bc_screen = barycentric2(pts2.row(0),pts2.row(1),pts2.row(2),P);
            Eigen::Vector3f bc_clip;
            bc_clip<< bc_screen[0]/pts(0, 3), bc_screen[1]/pts(1, 3), bc_screen[2]/pts(2, 3);

            float ttemp=bc_clip.sum();
            bc_clip<<bc_clip/ttemp;
            // bc_clip.normalize();

            Eigen::Vector3f zdepth;
            zdepth=clipc.row(2);
            float frag_depth= zdepth.dot(bc_clip);

            if(bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2]<0||zbuffer[P[0]+P[1]*image.get_width()]>frag_depth) continue;
            
            TGAColor color;
            Vec3f bt(bc_clip[0], bc_clip[1], bc_clip[2]);
            bool     discard = shader.fragment(bt, color);
            if (!discard)
            {
                zbuffer[P[0] + P[1] * image.get_width()] = frag_depth;
                image.set(P[0], P[1], color);
            }

        }
    }
}