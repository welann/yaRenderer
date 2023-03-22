#include"../include/geometry.h"
#include"../include/tgaimage.h"

#include<Eigen/Dense>



struct IShader
{
    virtual ~IShader(){};
    virtual Eigen::Vector4f vertex(int iface, int nthvert)       = 0;
    virtual bool  fragment(Vec3f bar, TGAColor &color) = 0;
};