#include "../include/model.h"
#include "IShader.h"
#include "Amodel.h"

#include <Eigen/Dense>

// extern Model  *model;
extern Amodel *amodel;

extern Eigen::Matrix4f ModelViewMatrix;
extern Eigen::Matrix4f ViewportMatrix;
extern Eigen::Matrix4f ProjectionMatrix;

struct Shader : public IShader
{
    mat<2, 3, float> varying_uv; // triangle uv coordinates, written by the vertex shader, read by the fragment shader

    Eigen::Matrix<float, 2, 3> varying_uvMatrix;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    Eigen::Matrix<float, 4, 3> varying_triMatrix; // triangle coordinates (screen space), written by VS, read by FS

    virtual Eigen::Vector4f vertex(int iface, int nthvert)
    {
        // varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        varying_uvMatrix.col(nthvert) = amodel->uv(iface, nthvert);

        // Vec3f           vert = model->vert(iface, nthvert);
        Eigen::Vector3f vert =amodel->vert(iface, nthvert);
        Eigen::Vector4f inflateVert;
        inflateVert << vert, 1.0f;
        Eigen::Vector4f Gl_Vertex = ProjectionMatrix * ModelViewMatrix * inflateVert;

        varying_triMatrix.col(nthvert) = Gl_Vertex;

        return Gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color)
    {
        Vec2f uv = varying_uv * bar;

        Eigen::Vector3f temp(bar.x, bar.y, bar.z);
        Eigen::Vector2f euv=varying_uvMatrix * temp;

        // color = model->diffuse(uv);
        color=amodel->diffuse(euv);
        return false;
    }
};
