#include <stdint.h>

#include "video.h"
#include "scroll.h"
#include "blitter.h"
#include "gfx.h"
#include "world.h"
#include "dino.h"
#include "palette.h"
#include "tos.h"

/* The live, VBASE-scanned copy of the world. world_pixels (world.c)
 * stays pristine and is only ever read from, so the dino head can be
 * stamped into world_live each frame and cleanly erased again next
 * frame by re-copying from the untouched original. */
static uint16_t world_live[WORLD_H * WORLD_WORDS_PER_LINE] __attribute__((aligned(256)));

#define SCROLL_SPEED_PX 1  /* pixels of world motion per frame */

int main(void)
{
    long super_token;
    uint16_t palette[16];
    planar_bitmap_t world_bmp;
    uint16_t scroll_px = 0;
    int frame_no = 0;

    super_token = video_init();

    palette_build(palette);
    video_set_palette(palette);

    world_bmp.words = world_pixels;
    world_bmp.width_px = WORLD_W;
    world_bmp.height_px = WORLD_H;
    world_bmp.words_per_line = WORLD_WORDS_PER_LINE;
    world_generate(&world_bmp);

    blit_copy_rect(world_live, WORLD_WORDS_PER_LINE,
                    world_pixels, WORLD_WORDS_PER_LINE,
                    WORLD_WORDS_PER_LINE, WORLD_H);

    dino_init();

    video_set_base(world_live);

    for (;;) {
        video_wait_vbl();

        dino_frame(world_live, WORLD_WORDS_PER_LINE,
                   world_pixels, WORLD_WORDS_PER_LINE,
                   scroll_px, frame_no);

        scroll_set_offset(scroll_px);

        scroll_px = (uint16_t)((scroll_px + SCROLL_SPEED_PX) % SCROLL_PERIOD_PX);
        frame_no++;

        if (tos_key_pressed()) {
            tos_read_key();
            break;
        }
    }

    video_shutdown(super_token);
    return 0;
}
