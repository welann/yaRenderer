#include <glm/glm.hpp>
#include "graphics.h"


using namespace glm;

enum class usage_t
{
    USAGE_LDR_COLOR,
    USAGE_LDR_DATA,
    USAGE_HDR_COLOR,
    USAGE_HDR_DATA
};

struct texture_t
{
    int   width, height;
    vec4 *buffer;
} ;

struct cubemap_t
{
    texture_t *faces[6];
} ;

/* texture related functions */
texture_t *texture_create(int width, int height);
void       texture_release(texture_t *texture);
texture_t *texture_from_file(const char *filename, usage_t usage);
void       texture_from_colorbuffer(texture_t *texture, framebuffer_t *framebuffer);
void       texture_from_depthbuffer(texture_t *texture, framebuffer_t *framebuffer);
vec4       texture_repeat_sample(texture_t *texture, vec2 texcoord);
vec4       texture_clamp_sample(texture_t *texture, vec2 texcoord);
vec4       texture_sample(texture_t *texture, vec2 texcoord);

/* cubemap related functions */
cubemap_t *cubemap_from_files(const char *positive_x, const char *negative_x,
                              const char *positive_y, const char *negative_y,
                              const char *positive_z, const char *negative_z,
                              usage_t usage);
void       cubemap_release(cubemap_t *cubemap);
vec4     cubemap_repeat_sample(cubemap_t *cubemap, vec3 direction);
vec4     cubemap_clamp_sample(cubemap_t *cubemap, vec3 direction);
vec4     cubemap_sample(cubemap_t *cubemap, vec3 direction);

