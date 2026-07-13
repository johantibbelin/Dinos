#ifndef SCROLL_H
#define SCROLL_H

#include <stdint.h>

/*
 * STE hardware fine horizontal scroll.
 *
 * The background world buffer is drawn once as a single tile that is
 * seamless every SCROLL_PERIOD_PX pixels (see world.h). To scroll the
 * display we never touch the video base address or redraw anything:
 * we just advance a pixel offset 0..(SCROLL_PERIOD_PX-1) and program
 * it into the shifter every frame, wrapping for free because the tile
 * repeats exactly on that period.
 *
 * The offset splits into two hardware pieces:
 *   - LINEWID steps in whole 16-pixel groups (4 interleaved plane
 *     words per group in low resolution).
 *   - HSCROLL provides the remaining 0-15 pixel fine offset.
 */
#define SCROLL_PERIOD_PX 320

void scroll_set_offset(uint16_t pixel_offset);

#endif /* SCROLL_H */
