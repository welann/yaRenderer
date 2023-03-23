#include "../stb_image.h"
#include "tgaimage.h"
#include <iostream>
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

enum class TextureKind
{
    diffuse,
    normal,
    specular
};

struct material
{
    TGAImage diffuse_map;
    TGAImage normal_map;
    TGAImage specular_map;
    Color    color;
    Color    specular;
    float    gloss;
    float    bump_scale;

    TGAImage* operator[](const TextureKind kind)
    {
        switch (kind)
        {
            case TextureKind::diffuse:
                return &diffuse_map;
                break;
            case TextureKind::normal:
                return &normal_map;
                break;
            case TextureKind::specular:
                return &specular_map;
                break;

            default: return nullptr;
        }
    }
    void setTexture(const TextureKind kind, TGAImage& image)
    {
        switch (kind)
        {
            case TextureKind::diffuse:
                diffuse_map = image;
                std::cout << "set diffuse map succeed" << std::endl;
                break;
            case TextureKind::normal:
                std::cout << "set normal map succeed" << std::endl;
                normal_map = image;
                break;
            case TextureKind::specular:
                specular_map = image;
                std::cout << "set specular map succeed" << std::endl;
                break;

            default: std::cout << "reset Texture failed" << std::endl;
        }
    }
};
