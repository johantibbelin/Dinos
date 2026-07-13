#ifndef TOS_H
#define TOS_H

/*
 * Minimal GEMDOS/BIOS/XBIOS trap wrappers.
 *
 * We avoid pulling in a full C library and just implement the handful
 * of TOS calls this demo needs directly with inline 68000 assembly,
 * using the standard trap calling convention: push arguments right to
 * left, push the function number, TRAP, then the caller pops the
 * argument words off the stack. Return value comes back in d0.
 */

#include <stdint.h>

static inline long tos_super(void *new_ssp)
{
    register long d0 __asm__("d0");
    __asm__ volatile(
        "move.l %1,-(sp)\n\t"
        "move.w #0x20,-(sp)\n\t"
        "trap   #14\n\t"
        "addq.l #6,sp"
        : "=r"(d0)
        : "g"(new_ssp)
        : "cc", "memory");
    return d0;
}

static inline void tos_vsync(void)
{
    __asm__ volatile(
        "move.w #0x25,-(sp)\n\t"
        "trap   #14\n\t"
        "addq.l #2,sp"
        : : : "cc", "memory");
}

static inline int16_t tos_bconstat(int16_t dev)
{
    register long d0 __asm__("d0");
    __asm__ volatile(
        "move.w %1,-(sp)\n\t"
        "move.w #0x01,-(sp)\n\t"
        "trap   #13\n\t"
        "addq.l #4,sp"
        : "=r"(d0)
        : "g"(dev)
        : "cc", "memory");
    return (int16_t)d0;
}

static inline int16_t tos_bconin(int16_t dev)
{
    register long d0 __asm__("d0");
    __asm__ volatile(
        "move.w %1,-(sp)\n\t"
        "move.w #0x02,-(sp)\n\t"
        "trap   #13\n\t"
        "addq.l #4,sp"
        : "=r"(d0)
        : "g"(dev)
        : "cc", "memory");
    return (int16_t)d0;
}

/* Enter supervisor mode; returns a token to restore the previous mode. */
static inline long tos_super_enter(void)  { return tos_super((void *)0); }
static inline void tos_super_exit(long tok) { tos_super((void *)tok); }

/* Non-blocking check for a key press on the keyboard device (2). */
static inline int tos_key_pressed(void) { return tos_bconstat(2) != 0; }
static inline int tos_read_key(void)    { return tos_bconin(2); }

#endif /* TOS_H */
