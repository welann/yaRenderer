#pragma once


enum class format_t {
    FORMAT_LDR,
    FORMAT_HDR
};


struct image_t {
    format_t format;
    int width, height, channels;
    unsigned char *ldr_buffer;
    float *hdr_buffer;
} ;

/* image creating/releasing */
image_t *image_create(int width, int height, int channels, format_t format);
void image_release(image_t *image);
image_t *image_load(const char *filename);
void image_save(image_t *image, const char *filename);

/* image processing */
void image_flip_h(image_t *image);
void image_flip_v(image_t *image);