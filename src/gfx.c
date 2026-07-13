#include "gfx.h"

void gfx_set_pixel(planar_bitmap_t *bmp, int x, int y, int color)
{
    int group, bit, line_base, group_base, p;
    uint16_t mask;

    if (x < 0 || y < 0 || x >= bmp->width_px || y >= bmp->height_px)
        return;

    group      = x >> 4;
    bit        = 15 - (x & 15);
    line_base  = y * bmp->words_per_line;
    group_base = line_base + group * 4;
    mask       = (uint16_t)(1u << bit);

    for (p = 0; p < 4; p++) {
        if (color & (1 << p))
            bmp->words[group_base + p] |= mask;
        else
            bmp->words[group_base + p] = (uint16_t)(bmp->words[group_base + p] & ~mask);
    }
}

void gfx_clear(planar_bitmap_t *bmp, int color)
{
    int x, y;
    for (y = 0; y < bmp->height_px; y++)
        for (x = 0; x < bmp->width_px; x++)
            gfx_set_pixel(bmp, x, y, color);
}

void gfx_fill_rect(planar_bitmap_t *bmp, int x0, int y0, int x1, int y1, int color)
{
    int x, y;
    if (x1 < x0) { int t = x0; x0 = x1; x1 = t; }
    if (y1 < y0) { int t = y0; y0 = y1; y1 = t; }
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++)
            gfx_set_pixel(bmp, x, y, color);
}

/* Brute-force filled triangle: half-plane test over the bounding box.
 * Only used at startup to build small procedural sprites, so raw
 * per-pixel cost is not a concern. */
void gfx_fill_tri(planar_bitmap_t *bmp, int x0, int y0, int x1, int y1, int x2, int y2, int color)
{
    int minx, maxx, miny, maxy, x, y;
    long d1, d2, d3;
    int has_neg, has_pos;

    minx = x0; if (x1 < minx) minx = x1; if (x2 < minx) minx = x2;
    maxx = x0; if (x1 > maxx) maxx = x1; if (x2 > maxx) maxx = x2;
    miny = y0; if (y1 < miny) miny = y1; if (y2 < miny) miny = y2;
    maxy = y0; if (y1 > maxy) maxy = y1; if (y2 > maxy) maxy = y2;

    for (y = miny; y <= maxy; y++) {
        for (x = minx; x <= maxx; x++) {
            d1 = (long)(x - x1) * (y0 - y1) - (long)(x0 - x1) * (y - y1);
            d2 = (long)(x - x2) * (y1 - y2) - (long)(x1 - x2) * (y - y2);
            d3 = (long)(x - x0) * (y2 - y0) - (long)(x2 - x0) * (y - y0);

            has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
            has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

            if (!(has_neg && has_pos))
                gfx_set_pixel(bmp, x, y, color);
        }
    }
}

/* Membership test (x/rx)^2 + (y/ry)^2 <= 1, scaled to integers as
 * (x*ry)^2 + (y*rx)^2 <= (rx*ry)^2 to avoid floating point. */
void gfx_fill_ellipse(planar_bitmap_t *bmp, int cx, int cy, int rx, int ry, int color)
{
    int x, y;
    long rxry2;

    if (rx <= 0 || ry <= 0)
        return;

    rxry2 = (long)rx * rx * (long)ry * ry;

    for (y = -ry; y <= ry; y++) {
        for (x = -rx; x <= rx; x++) {
            long v = (long)x * x * (long)ry * ry + (long)y * y * (long)rx * rx;
            if (v <= rxry2)
                gfx_set_pixel(bmp, cx + x, cy + y, color);
        }
    }
}
