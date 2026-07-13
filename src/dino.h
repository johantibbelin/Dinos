#ifndef DINO_HEADER_H
#define DINO_HEADER_H

#include <stdint.h>

/*
 * The dino head/neck you're riding, always drawn at a fixed spot near
 * the bottom of the screen while the world hardware-scrolls past.
 *
 * Because the background's smooth per-pixel motion comes from the
 * STE's HSCROLL register (which shifts an entire scanline, sprite
 * included), keeping the head visually still needs pixel-accurate
 * placement, not just whole-word blits. Rather than fight the
 * blitter's word-aligned addressing at runtime, we precompute 16
 * horizontally shifted "phases" of the sprite and its punch mask once
 * at startup (plain CPU bit-shifting, done carefully so it doesn't
 * depend on any blitter timing quirk), each phase already padded out
 * to a whole number of words. Per frame we just pick the phase that
 * matches the current fine-scroll value and blit it at a word-aligned
 * screen column -- ordinary, safe, word-aligned blitter work.
 */
#define DINO_SCREEN_X 112   /* fixed on-screen column, word aligned */
#define DINO_Y_TOP    128   /* top row of the padded restore rect   */

void dino_init(void);

/* Restore whatever the head previously overdrew, then composite the
 * head at the current scroll phase and gallop-bob frame into `live`.
 * `master`/`master_stride` is the clean, never-modified background
 * used as the restore source. All strides are in 16-bit words. */
void dino_frame(uint16_t *live, uint16_t live_stride,
                 const uint16_t *master, uint16_t master_stride,
                 uint16_t scroll_px, int frame_no);

#endif /* DINO_HEADER_H */
