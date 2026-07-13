#ifndef WORLD_HEADER_H
#define WORLD_HEADER_H

#include <stdint.h>
#include "gfx.h"

/*
 * The scrolling background: sky with clouds on top, a pine forest
 * silhouette, and a near-ground band at the bottom. It is built once
 * at startup into a buffer twice as wide as the screen. The left half
 * (0..SCROLL_PERIOD_PX-1) holds the unique hand-composed content; the
 * right half is an exact blitter copy of the left half, so the
 * hardware scroll offset can cycle 0..SCROLL_PERIOD_PX-1 forever with
 * a seamless wrap back to 0 (see scroll.h).
 */
#define WORLD_W 640
#define WORLD_H 200
#define WORLD_WORDS_PER_LINE ((WORLD_W / 16) * 4)

#define WORLD_HORIZON_Y   160  /* first row of the near-ground band */
#define WORLD_TREE_BASE_Y 159  /* baseline the tree silhouettes sit on */

extern uint16_t world_pixels[]; /* WORLD_H * (WORLD_W/16*4) words */

void world_generate(planar_bitmap_t *world);

#endif /* WORLD_HEADER_H */
