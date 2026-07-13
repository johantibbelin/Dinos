# Dinos

Techdemo: first-person dinoride on Atari STE.

Sky with drifting clouds up top, a scrolling pine forest silhouette
below it, and the head/neck of the dino you're riding fixed near the
bottom of the screen. The world is a single 320x200x16 low-resolution
buffer scrolled with the STE's hardware fine-scroll registers; the
blitter draws the dino head every frame and duplicates/restores
background tiles.

## Building

You need an Atari ST/STE cross compiler, e.g.
[`m68k-atari-mint-gcc`](https://tho-otto.de/crossmint.php) with
libcmini or mintlib on your `PATH`.

```
make            # builds DINO.PRG
make run        # builds, then launches it in Hatari if installed
```

If you don't have the cross toolchain handy, `make check` runs a
syntax-only pass over the sources with a generic m68k GCC
(`gcc-m68k-linux-gnu` on Debian/Ubuntu) so you can still validate C
changes. It does not produce a runnable Atari binary.

Run `DINO.PRG` on real STE hardware or in an STE-mode emulator (e.g.
[Hatari](https://hatari.tuxfamily.org/)). Press any key to quit.

## Technical notes

- **Resolution**: 320x200, 4 bitplanes (16 colours), standard STE
  low-res planar interleaved format.
- **Hardware scroll**: the background lives in a 640-pixel-wide buffer
  (`world.c`) whose right half is a blitter copy of the left half, so
  scrolling the STE's `LINEWID` ($FFFF820E) / `HSCROLL` ($FFFF8265)
  registers through one period (320px) wraps seamlessly forever. See
  `src/scroll.c`.
- **Blitter**: used for (1) the initial background duplication, (2)
  restoring the patch of background the dino head previously overdrew,
  and (3) compositing the head onto the screen every frame via the
  standard AND-mask-then-OR-sprite technique (`src/blitter.c`,
  `src/dino.c`).
- **Fixed dino head over a smoothly scrolling world**: since STE
  `HSCROLL` shifts an entire scanline (including anything blitted into
  it), keeping the head pixel-still while the background scrolls
  smoothly needs the head positioned with the same sub-pixel accuracy
  as the background -- but the blitter in this codebase only issues
  plain word-aligned copies. `dino.c` sidesteps that by precomputing 16
  horizontally shifted phases of the sprite/mask at startup (plain
  CPU bit-shifting) and picking the matching phase each frame, so the
  runtime blits stay simple and word-aligned.

## Known limitations / things to double check on real hardware

This was written and syntax-checked (via `make check`, a generic m68k
GCC) in a sandbox without an Atari cross toolchain, Atari hardware, or
an emulator available, so it has **not** been run on a real STE or in
Hatari. The logic and register usage follow the standard, widely
published STE hardware-scroll and blitter techniques, but if something
looks wrong on actual hardware, check these first:

- The exact STE `LINEWID`/`HSCROLL` register addresses and the
  documented "write `HSCROLL` twice" erratum workaround in
  `src/scroll.c` / `src/video.c`.
- The blitter `HOP`/`LOP` truth-table bit ordering used for
  `LOP_AND`/`LOP_REPLACE`/`LOP_OR` in `src/blitter.h`.
- The STE palette register format: this demo intentionally sticks to
  the plain ST-compatible 3-bit-per-gun encoding (`RGB3()` in
  `src/video.h`) rather than the STE's extra 4th colour bit, to avoid
  an unverified bit-swizzle.
