#ifndef STE_H
#define STE_H

/*
 * Atari STE hardware register map used by this demo.
 *
 * Addresses follow the standard Atari ST/STE shifter and blitter
 * register layout as published in the Atari Compendium and the
 * classic demoscene "hardware scroll" tutorials. The two STE-only
 * scroll registers (LINEWID / HSCROLL) have a well known write
 * erratum: HSCROLL must be written twice in a row for the value to
 * reliably latch, which scroll.c works around.
 */

#include <stdint.h>

/* ---- Shifter / video registers ---------------------------------- */
#define VID_VBASE_HI   (*(volatile uint8_t  *)0xFFFF8201) /* screen base A23-A16 */
#define VID_VBASE_MED  (*(volatile uint8_t  *)0xFFFF8203) /* screen base A15-A8  */
#define VID_VBASE_LOW  (*(volatile uint8_t  *)0xFFFF820DUL) /* screen base A7-A1, STE only */

#define VID_VCOUNT_HI  (*(volatile uint8_t  *)0xFFFF8205) /* read-only */
#define VID_VCOUNT_MED (*(volatile uint8_t  *)0xFFFF8207)
#define VID_VCOUNT_LOW (*(volatile uint8_t  *)0xFFFF8209)

#define VID_SYNCMODE   (*(volatile uint8_t  *)0xFFFF820A)

#define VID_LINEWID    (*(volatile uint8_t  *)0xFFFF820EUL) /* STE: extra words/line */
#define VID_RESOLUTION (*(volatile uint16_t *)0xFFFF8260)   /* 0 = 320x200x16 low-res */
#define VID_HSCROLL    (*(volatile uint8_t  *)0xFFFF8265UL) /* STE: 0-15 px fine scroll */

#define VID_PALETTE    ((volatile uint16_t *)0xFFFF8240)    /* 16 colour registers */

/* ---- Blitter registers -------------------------------------------- */
#define BLIT_HALFTONE  ((volatile uint16_t *)0xFFFF8A00)    /* 16 words */
#define BLIT_SRC_XINC  (*(volatile int16_t *)0xFFFF8A20)
#define BLIT_SRC_YINC  (*(volatile int16_t *)0xFFFF8A22)
#define BLIT_SRC_ADDR  (*(volatile uint32_t*)0xFFFF8A24)
#define BLIT_ENDMASK1  (*(volatile uint16_t*)0xFFFF8A28)
#define BLIT_ENDMASK2  (*(volatile uint16_t*)0xFFFF8A2A)
#define BLIT_ENDMASK3  (*(volatile uint16_t*)0xFFFF8A2C)
#define BLIT_DST_XINC  (*(volatile int16_t *)0xFFFF8A2E)
#define BLIT_DST_YINC  (*(volatile int16_t *)0xFFFF8A30)
#define BLIT_DST_ADDR  (*(volatile uint32_t*)0xFFFF8A32)
#define BLIT_XCOUNT    (*(volatile uint16_t*)0xFFFF8A36)    /* words per line */
#define BLIT_YCOUNT    (*(volatile uint16_t*)0xFFFF8A38)    /* line count */
#define BLIT_HOP       (*(volatile uint8_t *)0xFFFF8A3A)    /* halftone op, bits 0-1 */
#define BLIT_LOP       (*(volatile uint8_t *)0xFFFF8A3B)    /* logic op, bits 0-3 */
#define BLIT_CONTROL   (*(volatile uint8_t *)0xFFFF8A3C)    /* bit7=busy bit6=hog */
#define BLIT_SKEW      (*(volatile uint8_t *)0xFFFF8A3D)

#endif /* STE_H */
