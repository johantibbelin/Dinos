#include "video.h"
#include "ste.h"
#include "tos.h"

/* Enter supervisor mode and switch the shifter into 320x200x16
 * low-resolution mode with a plain 50Hz sync. Returns the token
 * needed to leave supervisor mode again via video_shutdown(). */
long video_init(void)
{
    long token = tos_super_enter();

    VID_SYNCMODE   = 0x02; /* standard 50Hz internal sync */
    VID_RESOLUTION = 0x00; /* low resolution */
    VID_LINEWID    = 0x00;
    VID_HSCROLL    = 0x00;
    VID_HSCROLL    = 0x00; /* STE erratum: write twice to latch */

    return token;
}

void video_shutdown(long super_token)
{
    tos_super_exit(super_token);
}

void video_set_base(const void *buffer)
{
    uint32_t addr = (uint32_t)(uintptr_t)buffer;

    VID_VBASE_HI  = (uint8_t)(addr >> 16);
    VID_VBASE_MED = (uint8_t)(addr >> 8);
    VID_VBASE_LOW = (uint8_t)(addr & 0xFE); /* STE: word aligned */
}

void video_set_palette(const uint16_t *colors16)
{
    int i;
    for (i = 0; i < 16; i++)
        VID_PALETTE[i] = colors16[i];
}

void video_wait_vbl(void)
{
    tos_vsync();
}
