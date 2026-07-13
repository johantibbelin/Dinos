#include "scroll.h"
#include "ste.h"

void scroll_set_offset(uint16_t pixel_offset)
{
    uint16_t px = pixel_offset % SCROLL_PERIOD_PX;
    uint8_t  groups_of_16 = (uint8_t)(px / 16);
    uint8_t  fine = (uint8_t)(px % 16);

    /* 4 bitplanes interleaved => one 16px group is 4 words. */
    VID_LINEWID = (uint8_t)(groups_of_16 * 4);

    /* Known STE erratum: HSCROLL only reliably latches when written
     * twice in succession. */
    VID_HSCROLL = fine;
    VID_HSCROLL = fine;
}
