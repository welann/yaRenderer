#include <vector>

#include <iostream>

#include <cmath>

#include <limits>

#include "tgaimage.h"

#include "model.h"

// #include "geometry.h"

  

#include <Eigen/Dense>

using namespace std;

using namespace Eigen;

  

const int width  = 800;

const int height = 800;

const int depth  = 255;

  

Model *model   = NULL;

int   *zbuffer = NULL;

  

// Vec3f light_dir = Vec3f(1,-1,1).normalize();

// Vec3f eye(1,1,3);

// Vec3f center(0,0,0);

  

Vector3d light_dir = Vector3d(1, -1, 1).normalized();

Vector3d eye(1, 1, 3);

Vector3d center(0, 0, 0);

  

Matrix4d viewport(int x, int y, int w, int h)

{

    Matrix4d m = MatrixXd::Identity(4, 4);

    m(0, 3)    = x + w / 2.f;

    m(1, 3)    = y + h / 2.f;

    m(2, 3)    = depth / 2.f;

  

    m(0, 0) = w / 2.f;

    m(1, 1) = h / 2.f;

    m(2, 2) = depth / 2.f;

    return m;

}

  

Matrix4d lookat(Vector3d eye, Vector3d center, Vector3d up)

{

    Vector3d z   = (eye - center).normalized();

    Vector3d x   = (up.cross(z)).normalized();

    Vector3d y   = (z.cross(x)).normalized();

    Matrix4d res = MatrixXd::Identity(4, 4);

    for (int i = 0; i < 3; i++)

    {

        res(0, i) = x[i];

        res(1, i) = y[i];

        res(2, i) = z[i];

        res(i, 3) = -center[i];

    }

    return res;

}

  

void triangle(Vector3d t0, Vector3d t1, Vector3d t2, float ity0, float ity1, float ity2, TGAImage &image, int *zbuffer)

{

    if (t0[1] == t1[1] && t0[1] == t2[1]) return; // i dont care about degenerate triangles

    if (t0[1] > t1[1])

    {

        std::swap(t0, t1);

        std::swap(ity0, ity1);

    }

    if (t0[1] > t2[1])

    {

        std::swap(t0, t2);

        std::swap(ity0, ity2);

    }

    if (t1[1] > t2[1])

    {

        std::swap(t1, t2);

        std::swap(ity1, ity2);

    }

  

    int total_height = t2[1] - t0[1];

    for (int i = 0; i < total_height; i++)

    {

        bool     second_half    = i > t1[1] - t0[1] || t1[1] == t0[1];

        int      segment_height = second_half ? t2[1] - t1[1] : t1[1] - t0[1];

        float    alpha          = (float)i / total_height;

        float    beta           = (float)(i - (second_half ? t1[1] - t0[1] : 0)) / segment_height; // be careful: with above conditions no division by zero here

        Vector3d A              = t0 + Vector3d(t2 - t0) * alpha;

        Vector3d B              = second_half ? t1 + Vector3d(t2 - t1) * beta : t0 + Vector3d(t1 - t0) * beta;

        float    ityA           = ity0 + (ity2 - ity0) * alpha;

        float    ityB           = second_half ? ity1 + (ity2 - ity1) * beta : ity0 + (ity1 - ity0) * beta;

        if (A[0] > B[0])

        {

            std::swap(A, B);

            std::swap(ityA, ityB);

        }

        for (int j = A[0]; j <= B[0]; j++)

        {

            float    phi  = B[0] == A[0] ? 1. : (float)(j - A[0]) / (B[0] - A[0]);

            Vector3d P    = Vector3d(A) + Vector3d(B - A) * phi;

            float    ityP = ityA + (ityB - ityA) * phi;

            int      idx  = P[0] + P[1] * width;

            if (P[0] >= width || P[1] >= height || P[0] < 0 || P[1] < 0) continue;

            if (zbuffer[idx] < P[2])

            {

                zbuffer[idx] = P[2];

                image.set(P[0], P[1], TGAColor{255, 255, 255} * ityP);

            }

        }

    }

}

  

int main(int argc, char **argv)

{

  

    model = new Model(R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\diablo3_pose\diablo3_pose.obj)");

  
  

    zbuffer = new int[width * height];

    for (int i = 0; i < width * height; i++)

    {

        zbuffer[i] = std::numeric_limits<int>::min();

    }

  

    { // draw the model

        Matrix4d ModelView  = lookat(eye, center, Vector3d(0, 1, 0));

        Matrix4d Projection = MatrixXd::Identity(4, 4);

        Matrix4d ViewPort   = viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);

        Projection(3, 2)    = -1.f / (eye - center).norm();

  

        std::cerr << ModelView << std::endl;

        std::cerr << Projection << std::endl;

        std::cerr << ViewPort << std::endl;

        Matrix4d z = (ViewPort * Projection * ModelView);

        std::cerr << z << std::endl;

  

        Matrix4d mvp = ViewPort * Projection * ModelView;

  

        TGAImage image(width, height, TGAImage::RGB);

        for (int i = 0; i < model->nfaces(); i++)

        {

            std::vector<int> face = model->face(i);

            Vector3d            screen_coords[3];

            Vector3d            world_coords[3];

            float            intensity[3];

            for (int j = 0; j < 3; j++)

            {

                auto temp=model->vert(face[j]);

                Vector3d v(temp.x, temp.y, temp.z);

                Vector4d  v4;

                v4<<temp.x, temp.y, temp.z,1;

                Vector4d tee=mvp * v4;

                screen_coords[j] = Vector3d(tee(0),tee(1),tee(2));

                world_coords[j]  = v;

                auto temp2=model->norm(i, j);

                Vector3d normal(temp2.x, temp2.y, temp2.z);

                intensity[j]     =  normal.dot(light_dir) ;

            }

            triangle(screen_coords[0], screen_coords[1], screen_coords[2], intensity[0], intensity[1], intensity[2], image, zbuffer);

        }

        image.flip_vertically(); // i want to have the origin at the left bottom corner of the image

        image.write_tga_file("output.tga");

    }

  

    { // dump z-buffer (debugging purposes only)

        TGAImage zbimage(width, height, TGAImage::GRAYSCALE);

        for (int i = 0; i < width; i++)

        {

            for (int j = 0; j < height; j++)

            {

                zbimage.set(i, j, TGAColor(zbuffer[i + j * width],zbuffer[i + j * width],zbuffer[i + j * width]));

            }

        }

        zbimage.flip_vertically(); // i want to have the origin at the left bottom corner of the image

        zbimage.write_tga_file("zbuffer.tga");

    }

    delete model;

    delete[] zbuffer;

    return 0;

}