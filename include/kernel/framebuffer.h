#include <stdint.h>
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

typedef struct {
  uint32_t width;
  uint32_t height;
  uint32_t pitch;        // Number of bytes per each row   
  void * buf;
  uint32_t buf_size;
  uint32_t chars_width;  // Width in characters
  uint32_t chars_height; // Height in characters
  uint32_t chars_x;      // Character cursor position
  uint32_t chars_y;      // Character cursor position
} fb_info_t;

extern fb_info_t fbinfo;

int framebuffer_init(void); // Initializes fbinfo

#define COLORDEPTH 24
#define BYTES_PER_PIXEL COLORDEPTH/8
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} color_t;

void write_pixel(color_t * pxl, uint32_t x, uint32_t y);
void black_screen(void);
void fb_putc(char c);

#endif
