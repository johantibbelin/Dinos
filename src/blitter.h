#ifndef BLITTER_H
#define BLITTER_H

#include <stdint.h>

/*
 * Thin wrapper around the STE blitter coprocessor.
 *
 * The blitter moves 16-bit words from src to dst, combining them
 * through a logic op (LOP) selected truth table. Source and
 * destination line stride is expressed in words. All copies used by
 * this demo are made of whole 16-bit words (no sub-word skew), so
 * endmask1/2/3 are always left wide open (0xFFFF).
 *
 * LOP truth table convention used here (MSB..LSB = S1D1,S1D0,S0D1,S0D0):
 *   LOP_ZERO    0x0  result = 0
 *   LOP_AND     0x8  result = src AND dst   (punch a hole for a mask)
 *   LOP_REPLACE 0xC  result = src           (plain opaque copy)
 *   LOP_OR      0xE  result = src OR dst    (paint through a punched hole)
 *   LOP_ONE     0xF  result = 1
 */
#define LOP_ZERO    0x0
#define LOP_AND     0x8
#define LOP_REPLACE 0xC
#define LOP_OR      0xE
#define LOP_ONE     0xF

#define HOP_ALL_ONE      0x0
#define HOP_HALFTONE     0x1
#define HOP_SOURCE       0x2
#define HOP_SOURCE_HALF  0x3

typedef struct {
    const uint16_t *src;
    uint16_t       *dst;
    int16_t         src_x_inc;   /* bytes to add after each word, normally 2 */
    int16_t         src_y_inc;   /* bytes to add after each line */
    int16_t         dst_x_inc;
    int16_t         dst_y_inc;
    uint16_t        words_per_line;
    uint16_t        lines;
    uint8_t         hop;
    uint8_t         lop;
    uint8_t         skew;        /* 0 for word-aligned copies */
    uint8_t         hog;         /* 1 = take the bus exclusively */
} blit_job_t;

void blit_run(const blit_job_t *job);

/* Convenience helper for a plain opaque rectangle copy between two
 * planar 4-bitplane interleaved buffers (both using HOP_SOURCE /
 * LOP_REPLACE), given buffer strides in words. */
void blit_copy_rect(uint16_t *dst, uint16_t dst_stride_words,
                     const uint16_t *src, uint16_t src_stride_words,
                     uint16_t words_wide, uint16_t lines_tall);

#endif /* BLITTER_H */
