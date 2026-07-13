#include "dino.h"
#include "gfx.h"
#include "palette.h"
#include "blitter.h"

#define DINO_W   80
#define DINO_H   64
#define DINO_GROUPS (DINO_W / 16)              /* 5 groups of 16px    */
#define DINO_WPL (DINO_GROUPS * 4)             /* words/line, master  */

#define PHASE_W   96
#define PHASE_GROUPS (PHASE_W / 16)            /* 6 groups of 16px    */
#define PHASE_WPL (PHASE_GROUPS * 4)           /* words/line, phases  */

#define PHASES 16
#define BOB_AMPLITUDE 4
#define PADDED_H (DINO_H + 2 * BOB_AMPLITUDE)

static const int8_t bob_table[8] = { 0, 1, 2, 4, 3, 2, 1, 0 };

static uint16_t dino_master_sprite[DINO_H * DINO_WPL];
static uint16_t dino_master_mask[DINO_H * DINO_WPL];   /* 0xFFFF = background, 0 = punched */

static uint16_t dino_phase_sprite[PHASES][DINO_H * PHASE_WPL];
static uint16_t dino_phase_mask[PHASES][DINO_H * PHASE_WPL];

/* ---- master artwork ------------------------------------------------ */

static void draw_outer_shape(planar_bitmap_t *bmp, int color)
{
    /* neck (trapezoid as two triangles) */
    gfx_fill_tri(bmp, 18, 63, 54, 63, 46, 28, color);
    gfx_fill_tri(bmp, 18, 63, 46, 28, 28, 28, color);
    /* head */
    gfx_fill_ellipse(bmp, 58, 26, 20, 16, color);
    /* jaw / snout */
    gfx_fill_tri(bmp, 50, 20, 78, 26, 50, 34, color);
    /* back spikes */
    gfx_fill_tri(bmp, 24, 50, 16, 42, 28, 44, color);
    gfx_fill_tri(bmp, 28, 38, 20, 30, 32, 34, color);
}

static void build_master(void)
{
    planar_bitmap_t sprite = { dino_master_sprite, DINO_W, DINO_H, DINO_WPL };
    planar_bitmap_t mask   = { dino_master_mask,   DINO_W, DINO_H, DINO_WPL };

    /* Mask: background survives everywhere (all 1s) except the outer
     * silhouette, which is punched to 0. */
    gfx_clear(&mask, 15);
    draw_outer_shape(&mask, 0);

    /* Sprite: transparent (0) canvas, outline-coloured silhouette,
     * then a smaller inset body colour on top for a rim-lit look,
     * plus a belly patch and an eye. */
    gfx_clear(&sprite, 0);
    draw_outer_shape(&sprite, COL_DINO_OUTLINE);

    gfx_fill_tri(&sprite, 21, 60, 51, 60, 44, 30, COL_DINO_BODY);
    gfx_fill_tri(&sprite, 21, 60, 44, 30, 31, 30, COL_DINO_BODY);
    gfx_fill_ellipse(&sprite, 58, 26, 17, 13, COL_DINO_BODY);
    gfx_fill_tri(&sprite, 52, 21, 75, 26, 52, 32, COL_DINO_BODY);

    gfx_fill_ellipse(&sprite, 36, 50, 9, 10, COL_DINO_BELLY);

    gfx_fill_ellipse(&sprite, 64, 20, 4, 4, COL_DINO_EYE_WHITE);
    gfx_fill_ellipse(&sprite, 65, 19, 2, 2, COL_DINO_EYE_PUPIL);
}

/* ---- phase generation ------------------------------------------------
 * Shift one bitplane's row of `src_words` words right by `shift` bits
 * (0-15), writing `src_words + 1` words to dst so the spilled-over
 * bits have somewhere to land. Both src and dst are accessed with a
 * stride of 4 words, since the 4 bitplanes are interleaved one word
 * per plane per 16-pixel group. */
static void shift_plane_row(const uint16_t *src, int src_words,
                             uint16_t *dst, int shift)
{
    int i;
    uint16_t prev = 0;

    for (i = 0; i < src_words; i++) {
        uint16_t cur = src[i * 4];
        dst[i * 4] = shift ? (uint16_t)((cur >> shift) | (prev << (16 - shift))) : cur;
        prev = cur;
    }
    dst[src_words * 4] = shift ? (uint16_t)(prev << (16 - shift)) : 0;
}

static void shift_bitmap(const uint16_t *src, uint16_t *dst, int shift)
{
    int row, plane;
    for (row = 0; row < DINO_H; row++) {
        const uint16_t *src_row = src + row * DINO_WPL;
        uint16_t *dst_row = dst + row * PHASE_WPL;
        for (plane = 0; plane < 4; plane++)
            shift_plane_row(src_row + plane, DINO_GROUPS, dst_row + plane, shift);
    }
}

static void build_phases(void)
{
    static uint16_t punch[DINO_H * DINO_WPL];
    static uint16_t shifted_punch[DINO_H * PHASE_WPL];
    int i, phase;

    for (i = 0; i < DINO_H * DINO_WPL; i++)
        punch[i] = (uint16_t)~dino_master_mask[i]; /* 1 = punched, 0 = background */

    for (phase = 0; phase < PHASES; phase++) {
        shift_bitmap(dino_master_sprite, dino_phase_sprite[phase], phase);

        shift_bitmap(punch, shifted_punch, phase);
        for (i = 0; i < DINO_H * PHASE_WPL; i++)
            dino_phase_mask[phase][i] = (uint16_t)~shifted_punch[i];
    }
}

void dino_init(void)
{
    build_master();
    build_phases();
}

/* ---- per-frame compositing ------------------------------------------ */

static void blit_lop(uint16_t *dst, uint16_t dst_stride,
                      const uint16_t *src, uint8_t lop)
{
    blit_job_t job;

    job.src = src;
    job.dst = dst;
    job.src_x_inc = 2;
    job.dst_x_inc = 2;
    job.src_y_inc = 0; /* phase buffers are tightly packed */
    job.dst_y_inc = (int16_t)((dst_stride - PHASE_WPL) * 2);
    job.words_per_line = PHASE_WPL;
    job.lines = DINO_H;
    job.hop = HOP_SOURCE;
    job.lop = lop;
    job.skew = 0;
    job.hog = 1;

    blit_run(&job);
}

void dino_frame(uint16_t *live, uint16_t live_stride,
                 const uint16_t *master, uint16_t master_stride,
                 uint16_t scroll_px, int frame_no)
{
    static int have_prev = 0;
    static uint16_t prev_word_col = 0;

    int group_steps = scroll_px / 16;
    int fine        = scroll_px % 16;
    uint16_t word_col = (uint16_t)(((DINO_SCREEN_X + group_steps * 16) / 16) * 4);
    int bob = bob_table[frame_no & 7];
    int draw_y = DINO_Y_TOP + BOB_AMPLITUDE + bob;

    if (!have_prev) {
        prev_word_col = word_col;
        have_prev = 1;
    }

    blit_copy_rect(live + (uint32_t)DINO_Y_TOP * live_stride + prev_word_col, live_stride,
                    master + (uint32_t)DINO_Y_TOP * master_stride + prev_word_col, master_stride,
                    PHASE_WPL, PADDED_H);

    if (word_col != prev_word_col) {
        blit_copy_rect(live + (uint32_t)DINO_Y_TOP * live_stride + word_col, live_stride,
                        master + (uint32_t)DINO_Y_TOP * master_stride + word_col, master_stride,
                        PHASE_WPL, PADDED_H);
    }

    blit_lop(live + (uint32_t)draw_y * live_stride + word_col,
              live_stride, dino_phase_mask[fine], LOP_AND);
    blit_lop(live + (uint32_t)draw_y * live_stride + word_col,
              live_stride, dino_phase_sprite[fine], LOP_OR);

    prev_word_col = word_col;
}
