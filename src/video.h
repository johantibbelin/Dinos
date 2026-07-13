#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

/* ST low resolution: 320x200, 4 bitplanes, 16 colours. */
#define SCREEN_W 320
#define SCREEN_H 200

/* RGB in the plain ST-compatible 3-bit-per-gun palette format (each
 * component 0-7). This is fully STE compatible; we skip the STE's
 * extra low-order colour bit for simplicity. */
#define RGB3(r, g, b) (uint16_t)(((r) & 7) << 8 | ((g) & 7) << 4 | ((b) & 7))

long video_init(void);
void video_shutdown(long super_token);
void video_set_base(const void *buffer);
void video_set_palette(const uint16_t *colors16);
void video_wait_vbl(void);

#endif /* VIDEO_H */
