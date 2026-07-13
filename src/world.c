#include "world.h"
#include "palette.h"
#include "scroll.h"
#include "blitter.h"

#define WORDS_PER_LINE WORLD_WORDS_PER_LINE

uint16_t world_pixels[WORLD_H * WORDS_PER_LINE] __attribute__((aligned(256)));

typedef struct { int x, base_y, height, half_w; } tree_t;

/* Ten pines spread across one scroll period, heights/widths hand
 * varied so the silhouette doesn't look mechanically regular. */
static const tree_t trees[] = {
    {  10, WORLD_TREE_BASE_Y, 46, 16 },
    {  42, WORLD_TREE_BASE_Y, 34, 12 },
    {  70, WORLD_TREE_BASE_Y, 52, 18 },
    { 108, WORLD_TREE_BASE_Y, 30, 11 },
    { 138, WORLD_TREE_BASE_Y, 44, 15 },
    { 168, WORLD_TREE_BASE_Y, 58, 20 },
    { 198, WORLD_TREE_BASE_Y, 36, 13 },
    { 228, WORLD_TREE_BASE_Y, 48, 16 },
    { 262, WORLD_TREE_BASE_Y, 32, 12 },
    { 296, WORLD_TREE_BASE_Y, 50, 17 },
};
#define NUM_TREES (int)(sizeof(trees) / sizeof(trees[0]))

typedef struct { int x, y, rx, ry; } cloud_t;

static const cloud_t clouds[] = {
    {  30, 24, 22, 9 },
    {  95, 40, 28, 11 },
    { 160, 18, 18, 7 },
    { 220, 34, 26, 10 },
    { 280, 22, 20, 8 },
};
#define NUM_CLOUDS (int)(sizeof(clouds) / sizeof(clouds[0]))

static void paint_sky(planar_bitmap_t *w)
{
    gfx_fill_rect(w, 0, 0, SCROLL_PERIOD_PX - 1, 39, COL_SKY_TOP);
    gfx_fill_rect(w, 0, 40, SCROLL_PERIOD_PX - 1, 79, COL_SKY_MID);
    gfx_fill_rect(w, 0, 80, SCROLL_PERIOD_PX - 1, WORLD_TREE_BASE_Y, COL_SKY_HORIZON);
}

static void paint_clouds(planar_bitmap_t *w)
{
    int i;
    for (i = 0; i < NUM_CLOUDS; i++) {
        const cloud_t *c = &clouds[i];
        gfx_fill_ellipse(w, c->x, c->y + 2, c->rx, c->ry, COL_CLOUD_SHADE);
        gfx_fill_ellipse(w, c->x - 3, c->y - 2, c->rx - 4, c->ry - 3, COL_CLOUD_LIT);
    }
}

static void paint_forest(planar_bitmap_t *w)
{
    int i;
    /* Far tree band: a flat strip just above the near silhouette to
     * fake depth before the near pines are drawn on top. */
    gfx_fill_rect(w, 0, WORLD_TREE_BASE_Y - 20, SCROLL_PERIOD_PX - 1, WORLD_TREE_BASE_Y, COL_FOREST_FAR);

    for (i = 0; i < NUM_TREES; i++) {
        const tree_t *t = &trees[i];
        int apex_y = t->base_y - t->height;
        gfx_fill_tri(w, t->x - t->half_w, t->base_y, t->x + t->half_w, t->base_y, t->x, apex_y, COL_FOREST_NEAR);
        /* A second, smaller tier gives the pine a layered look. */
        gfx_fill_tri(w, t->x - t->half_w + 4, t->base_y - t->height / 3,
                        t->x + t->half_w - 4, t->base_y - t->height / 3,
                        t->x, apex_y - 6, COL_FOREST_NEAR);
    }
}

static void paint_ground(planar_bitmap_t *w)
{
    int x, y;
    gfx_fill_rect(w, 0, WORLD_HORIZON_Y, SCROLL_PERIOD_PX - 1, WORLD_H - 1, COL_GROUND_A);
    /* Coarse checkerboard dither for a hint of ground texture; an 8px
     * period divides SCROLL_PERIOD_PX cleanly so the tile still wraps
     * seamlessly. */
    for (y = WORLD_HORIZON_Y; y < WORLD_H; y += 4)
        for (x = ((y / 4) & 1) * 4; x < SCROLL_PERIOD_PX; x += 8)
            gfx_fill_rect(w, x, y, x + 3, y + 3, COL_GROUND_B);
}

void world_generate(planar_bitmap_t *world)
{
    paint_sky(world);
    paint_clouds(world);
    paint_forest(world);
    paint_ground(world);

    /* Duplicate the unique left half into the right half with the
     * blitter so the hardware scroll offset can wrap seamlessly at
     * SCROLL_PERIOD_PX. */
    blit_copy_rect(world->words + (SCROLL_PERIOD_PX / 16) * 4,
                    world->words_per_line,
                    world->words,
                    world->words_per_line,
                    (SCROLL_PERIOD_PX / 16) * 4,
                    WORLD_H);
}
