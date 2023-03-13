#ifndef __OUR_GL_H__
#define __OUR_GL_H__
#include "tgaimage.h"
#include "geometry.h"

#include <Eigen/Dense>

extern Matrix ModelView;
extern Matrix Projection;
extern Matrix Viewport;

extern Eigen::Matrix4f ModelViewMatrix;
extern Eigen::Matrix4f ViewportMatrix;
extern Eigen::Matrix4f ProjectionMatrix;

const float   depth = 2000.f;

void viewport(int x, int y, int w, int h);
void projection(float coeff = 0.f); // coeff = -1/c
void lookat(Vec3f eye, Vec3f center, Vec3f up);

struct IShader
{
    virtual ~IShader();
    virtual Vec4f vertex(int iface, int nthvert)       = 0;
    virtual bool  fragment(Vec3f bar, TGAColor &color) = 0;
};

// void triangle(Vec4f *pts, IShader &shader, TGAImage &image, float *zbuffer);
void triangle(mat<4, 3, float> &pts, IShader &shader, TGAImage &image, float *zbuffer);

void triangle2(Eigen::Matrix<float, 4, 3> &clipc, IShader &shader, TGAImage &image, float *zbuffer);

#endif //__OUR_GL_H__
