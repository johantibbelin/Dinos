#ifndef PALETTE_H
#define PALETTE_H

#include <stdint.h>
#include "video.h"

/* 16-colour palette shared by the sky/forest world and the dino
 * sprite so both can be built and blitted without ever remapping
 * colour indices. */
enum {
    COL_SKY_TOP = 0,
    COL_SKY_MID,
    COL_SKY_HORIZON,
    COL_CLOUD_LIT,
    COL_CLOUD_SHADE,
    COL_FOREST_FAR,
    COL_FOREST_NEAR,
    COL_GROUND_A,
    COL_GROUND_B,
    COL_DINO_BODY,
    COL_DINO_OUTLINE,
    COL_DINO_BELLY,
    COL_DINO_EYE_WHITE,
    COL_DINO_EYE_PUPIL,
    COL_SPARE_1,
    COL_SPARE_2
};

static inline void palette_build(uint16_t out[16])
{
    out[COL_SKY_TOP]        = RGB3(1, 3, 7);
    out[COL_SKY_MID]        = RGB3(3, 5, 7);
    out[COL_SKY_HORIZON]    = RGB3(7, 6, 4);
    out[COL_CLOUD_LIT]      = RGB3(7, 7, 7);
    out[COL_CLOUD_SHADE]    = RGB3(5, 5, 7);
    out[COL_FOREST_FAR]     = RGB3(0, 2, 1);
    out[COL_FOREST_NEAR]    = RGB3(0, 3, 1);
    out[COL_GROUND_A]       = RGB3(1, 2, 0);
    out[COL_GROUND_B]       = RGB3(1, 3, 0);
    out[COL_DINO_BODY]      = RGB3(2, 3, 1);
    out[COL_DINO_OUTLINE]   = RGB3(0, 1, 0);
    out[COL_DINO_BELLY]     = RGB3(4, 5, 2);
    out[COL_DINO_EYE_WHITE] = RGB3(7, 7, 6);
    out[COL_DINO_EYE_PUPIL] = RGB3(0, 0, 0);
    out[COL_SPARE_1]        = RGB3(4, 4, 4);
    out[COL_SPARE_2]        = RGB3(0, 0, 0);
}

#endif /* PALETTE_H */
