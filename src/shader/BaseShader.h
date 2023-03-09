#include"our_gl.h"
#include<glm/glm.hpp>
#include"model.h"


extern glm::mat4 ModelView; // "OpenGL" state matrices
extern glm::mat4 Projection;

extern glm::vec3 light_dir; // light source

struct Shader : IShader
{
    const Model &model;
    glm::vec3    uniform_l;   // light direction in view coordinates
    glm::mat2x3  varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    glm::mat3    varying_nrm; // normal per vertex to be interpolated by FS
    glm::mat3    view_tri;    // triangle in view coordinates

    Shader(const Model &m) : model(m)
    {
        uniform_l = glm::normalize(proj<3>((ModelView * embed<4>(light_dir, 0.)))); // transform the light vector to view coordinates
    }

    virtual void vertex(const int iface, const int nthvert, glm::vec4 &gl_Position)
    {
        set_col(varying_uv, nthvert, model.uv(iface, nthvert));
        set_col(varying_nrm, nthvert, proj<3>(invert_transpose(ModelView) * embed<4>(model.normal(iface, nthvert))));
        gl_Position = ModelView * embed<4>(model.vert(iface, nthvert));
        set_col(view_tri, nthvert, proj<3>(gl_Position));
        gl_Position = Projection * gl_Position;
    }

    virtual bool fragment(const glm::vec3 bar, TGAColor &gl_FragColor)
    {
        glm::vec3 bn = normalize(varying_nrm * bar); // per-vertex normal interpolation
        glm::vec2 uv = varying_uv * bar;             // tex coord interpolation

        auto temp = glm::mat3{col(view_tri, 1) - col(view_tri, 0), col(view_tri, 2) - col(view_tri, 0), bn};

        glm::mat3 AI = glm::transpose(invert_transpose(temp));
        glm::vec3 i  = AI * glm::vec3{varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0};
        glm::vec3 j  = AI * glm::vec3{varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0};
        glm::mat3 B  = glm::transpose(glm::mat3{{glm::normalize(i), glm::normalize(j), bn}});

        glm::vec3 n     = normalize(B * model.normal(uv));                                   // transform the normal from the texture to the tangent space
        double    diff  = glm::dot(n, uniform_l) > 0. ? glm::dot(n, uniform_l) : 0.;         // diffuse light intensity
        glm::vec3 r     = glm::normalize((n * (glm::dot(n, uniform_l) * 2.0f)) - uniform_l); // reflected light direction, specular mapping is described here: https://github.com/ssloy/tinyrenderer/wiki/Lesson-6-Shaders-for-the-software-renderer
        float     temp2 = -r.z > 0. ? -r.z : 0;
        double    spec  = std::pow(temp2, 5 + sample2D(model.specular(), uv)[0]); // specular intensity, note that the camera lies on the z-axis (in view), therefore simple -r.z

        TGAColor c = sample2D(model.diffuse(), uv);
        for (int i : {0, 1, 2})
            gl_FragColor[i] = std::min<int>(10 + c[i] * (diff + spec), 255); // (a bit of ambient light, diff + spec), clamp the result

        return false; // the pixel is not discarded
    }
};

