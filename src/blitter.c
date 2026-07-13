#include "blitter.h"
#include "ste.h"

void blit_run(const blit_job_t *job)
{
    /* Every job in this codebase uses HOP_SOURCE, which never
     * consults the halftone RAM, so it is left uninitialised. */
    BLIT_SRC_ADDR  = (uint32_t)(uintptr_t)job->src;
    BLIT_DST_ADDR  = (uint32_t)(uintptr_t)job->dst;
    BLIT_SRC_XINC  = job->src_x_inc;
    BLIT_SRC_YINC  = job->src_y_inc;
    BLIT_DST_XINC  = job->dst_x_inc;
    BLIT_DST_YINC  = job->dst_y_inc;

    BLIT_ENDMASK1  = 0xFFFF;
    BLIT_ENDMASK2  = 0xFFFF;
    BLIT_ENDMASK3  = 0xFFFF;

    BLIT_XCOUNT    = job->words_per_line;
    BLIT_YCOUNT    = job->lines;

    BLIT_HOP       = job->hop;
    BLIT_LOP       = job->lop;
    BLIT_SKEW      = job->skew;

    /* Start the blit: bit7 = busy/start, bit6 = hog bus mode. */
    BLIT_CONTROL = 0x80 | (job->hog ? 0x40 : 0x00);

    /* In non-hog mode the CPU shares the bus with the blitter and the
     * busy bit clears itself once the blit finishes; poll for it. In
     * hog mode the blitter owns the bus outright and this loop simply
     * exits as soon as control is handed back. */
    while (BLIT_CONTROL & 0x80)
        ;
}

void blit_copy_rect(uint16_t *dst, uint16_t dst_stride_words,
                     const uint16_t *src, uint16_t src_stride_words,
                     uint16_t words_wide, uint16_t lines_tall)
{
    blit_job_t job;

    job.src = src;
    job.dst = dst;
    job.src_x_inc = 2;
    job.dst_x_inc = 2;
    /* y_inc is the byte offset to move from the end of one line to the
     * start of the next; since x_inc already walked words_wide words
     * forward, we only add the remaining stride. */
    job.src_y_inc = (int16_t)((src_stride_words - words_wide) * 2);
    job.dst_y_inc = (int16_t)((dst_stride_words - words_wide) * 2);
    job.words_per_line = words_wide;
    job.lines = lines_tall;
    job.hop = HOP_SOURCE;
    job.lop = LOP_REPLACE;
    job.skew = 0;
    job.hog = 1;

    blit_run(&job);
}
