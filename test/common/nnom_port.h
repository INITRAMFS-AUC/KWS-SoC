/*
 * test/common/nnom_port.h — KWS-SoC project-local override of NNoM's port
 * header.  We put this directory ahead of third_party/nnom/port in the
 * include path (see test/Makefile) so NNoM's `#include "nnom_port.h"` from
 * core/layers/backends picks up THIS file instead of the upstream one.
 *
 * Why we override: the upstream port routes nnom_memcpy / nnom_memset
 * through the libc names `memcpy` / `memset`.  We provide our own
 * (word-aligned) memcpy/memset for the bare-metal link, but for *very
 * small* copies (sizeof a few words) the function-call prologue exceeds
 * the inner-loop savings, and `-flto -Os` chose not to inline the more
 * complex word-aligned function (it inlined the byte-loop version we
 * had before, which is why the byte version measured 20M cycles faster
 * even though it does 4x the per-byte work).
 *
 * Routing nnom_{memcpy,memset} through the GCC __builtin_* forms lets
 * the compiler specialise PER CALL SITE:
 *   - constant size (sizeof(struct), small literals)  → inline lw/sw,
 *                                                       no call at all.
 *   - variable / unknown size                          → libcall to
 *                                                       memcpy/memset
 *                                                       (our impl).
 * Best of both: zero overhead for cheap copies, fast loop for the
 * genuinely-variable hot-path calls (Conv2D im2col / padding).
 *
 * Everything else here mirrors upstream third_party/nnom/port/nnom_port.h
 * verbatim — keep them in sync if NNoM is bumped.
 */

#ifndef __NNOM_PORT_H__
#define __NNOM_PORT_H__

/* NNOM_BARE_METAL: define to suppress libc includes and printf-based logging.
 * Used when building for bare-metal MCU targets (no OS, no libc stdio). */
#ifndef NNOM_BARE_METAL
#include <stdlib.h>
#include <stdio.h>
#endif

#include <string.h>  /* always needed: memset, memcpy */

/* use static memory */
//#define NNOM_USING_STATIC_MEMORY    // enable to use built in memory allocation on a large static memory block
                                     // must set buf using "nnom_set_static_buf()" before creating a model.

/* dynamic memory interfaces */
/* when libc is not available, you shall implement the below memory interfaces (libc equivalents). */
#ifndef NNOM_USING_STATIC_MEMORY
    #define nnom_malloc(n)      malloc(n)
    #define nnom_free(p)        free(p)
#endif

/* memory interface — KWS-SoC change vs upstream:
 * route through __builtin_{memcpy,memset} so GCC can inline at call
 * sites with constant sizes (eliminates prologue overhead for the many
 * small struct / shape copies NNoM does).  Variable-size calls fall
 * through to a real memcpy/memset libcall, which the bare-metal link
 * resolves to test/common/memcpy.c / memset.c (word-aligned). */
#define nnom_memset(p,v,s)        __builtin_memset(p,v,s)
#define nnom_memcpy(dst,src,len)  __builtin_memcpy(dst,src,len)

/* runtime & debug */
#define nnom_us_get()       0       // return a microsecond timestamp
#define nnom_ms_get()       0       // return a millisecond timestamp
#ifndef NNOM_LOG
  #ifdef NNOM_BARE_METAL
    #define NNOM_LOG(...)   /* suppressed: no printf on bare-metal */
  #else
    #define NNOM_LOG(...)   printf(__VA_ARGS__)
  #endif
#endif

/* NNoM configuration */
#define NNOM_BLOCK_NUM  	(8)		// maximum number of memory blocks, increase it when log request.
#define DENSE_WEIGHT_OPT 	(1)		// if used fully connected layer optimized weights.

//#define NNOM_TRUNCATE             // disable: backend ops use round to the nearest int (default). enable: floor

/* Backend format configuration */
//#define NNOM_USING_CHW            // uncomment if using CHW format. otherwise using default HWC format.
                                    // Notes, CHW is incompatible with CMSIS-NN.
                                    // CHW must be used when using hardware accelerator such as KPU in K210 chip

/* Backend selection */
//#define NNOM_USING_CMSIS_NN       // uncomment if use CMSIS-NN for optimation

#endif
