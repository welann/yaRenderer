#include "IShader.h"
#include "Model.h"

#include <Eigen/Dense>

// extern Model  *model;
extern Model *amodel;

extern Eigen::Matrix4f ModelViewMatrix;
extern Eigen::Matrix4f ViewportMatrix;
extern Eigen::Matrix4f ProjectionMatrix;

struct Shader : public IShader
{
    mat<2, 3, float> varying_uv; // triangle uv coordinates, written by the vertex shader, read by the fragment shader

    Eigen::Matrix<float, 2, 3> varying_uvMatrix;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    Eigen::Matrix<float, 4, 3> varying_triMatrix; // triangle coordinates (screen space), written by VS, read by FS

    bool discard_face(int iface){
        Eigen::Vector3f lookdir;
        lookdir<<ModelViewMatrix(2,0),ModelViewMatrix(2,1),ModelViewMatrix(2,2);
        std::cout <<"lookdir: "<< lookdir <<std::endl;

        Eigen::Vector3f vert0 =amodel->vert(iface, 0);
        Eigen::Vector3f vert1 =amodel->vert(iface, 1);
        Eigen::Vector3f vert2 =amodel->vert(iface, 2);

        Eigen::Vector3f normal=(vert2-vert0).cross(vert1-vert0);

        float temp=normal.dot(lookdir);
        if(temp>0) return true;
        return false;
    }


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

        Eigen::Vector3f temp(bar.x, bar.y, bar.z);
        Eigen::Vector2f euv=varying_uvMatrix * temp;
 
        color=amodel->diffuse(euv);
        // std::cout << (int)color.bgra[0]<<" : "<<(int)color.bgra[1]<< " : "<<(int)color.bgra[2]<< " : "<<(int)color.bgra[3]<<std::endl;
        return false;
    }
};
