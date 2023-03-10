#include <glm/glm.hpp>
#include <assimp/scene.h>

#include <vector>
#include <string>
#include "../external/stb_image.h"
#include "image.h"
#include "texture.h"

namespace ya {

class Model
{
    // const aiScene* scene=nullptr;

    std::vector<glm::vec3> verts{};    // // array of vertices
    std::vector<glm::vec2> tex_coord{}; //// per-vertex array of tex coords
    std::vector<glm::vec3> norms{};   //  // per-vertex array of normal vectors
    std::vector<int>       facet_vrt{};
    std::vector<int>       facet_tex{}; // per-triangle indices in the above arrays
    std::vector<int>       facet_nrm{};
    texture_t              diffusemap{};  // diffuse color texture
    texture_t              normalmap{};   // normal map texture
    texture_t              specularmap{}; // specular map texture
    void                   load_texture(const std::string filename, const std::string suffix, texture_t& img);

public:
    Model(const std::string filename);
    int              nverts() const;
    int              nfaces() const;
    glm::vec3        normal(const int iface, const int nthvert) const; // per triangle corner normal vertex
    glm::vec3        normal(const glm::vec2& uv) const;                // fetch the normal vector from the normal map texture
    glm::vec3        vert(const int i) const;
    glm::vec3        vert(const int iface, const int nthvert) const;
    glm::vec2        uv(const int iface, const int nthvert) const;
    const texture_t& diffuse() const { return diffusemap; }
    const texture_t& specular() const { return specularmap; }
};

} // namespace ya
