#include "../stb_image.h"
#include "tgaimage.h"

struct Color
{
    float r, g, b, a;
    Color();
    Color(float R, float G, float B, float A = 255);
    Color(const TGAColor& tga_color);

    static float MAX_CHANNEL_VALUE;
    static Color White;
    static Color Red;
    static Color Green;
    static Color Blue;
    static Color Black;

    float& operator[](const size_t i);

    float operator[](const size_t i) const;

    Color operator+(const Color& color) const;

    Color operator*(float intensity) const;

    Color operator*(const Color& color) const;
};

struct material
{
    TGAImage* diffuse_map;
    TGAImage* normal_map;
    TGAImage* specular_map;
    Color     color;
    Color     specular;
    float     gloss;
    float     bump_scale;
};
