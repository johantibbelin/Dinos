#ifndef GFX_H
#define GFX_H

#include <stdint.h>

/*
 * Software plotting primitives for standard ST/STE low-resolution
 * planar bitmaps: 4 bitplanes, interleaved a word at a time, one word
 * per plane per 16-pixel group (word0=plane0 .. word3=plane3, then
 * the next 16-pixel group). Bit 15 of each word is the leftmost
 * pixel. These run once at startup to procedurally build the sky,
 * forest and dino sprite bitmaps -- the actual per-frame rendering is
 * done by the blitter and the hardware scroll registers.
 */
typedef struct {
    uint16_t *words;        /* pixel data, words_per_line * height words */
    uint16_t  width_px;     /* must be a multiple of 16 */
    uint16_t  height_px;
    uint16_t  words_per_line; /* == (width_px/16) * 4 */
} planar_bitmap_t;

void gfx_clear(planar_bitmap_t *bmp, int color /* 0-15 */);
void gfx_set_pixel(planar_bitmap_t *bmp, int x, int y, int color);
void gfx_fill_rect(planar_bitmap_t *bmp, int x0, int y0, int x1, int y1, int color);
void gfx_fill_tri(planar_bitmap_t *bmp, int x0, int y0, int x1, int y1, int x2, int y2, int color);
void gfx_fill_ellipse(planar_bitmap_t *bmp, int cx, int cy, int rx, int ry, int color);

#endif /* GFX_H */
