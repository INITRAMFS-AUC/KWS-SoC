/* syscall_stubs.c — Bare-metal syscall stubs for newlib.
 *
 * Without a proxy kernel, any ecall from newlib (sbrk, write, exit, etc.)
 * causes the crt0.s spin trap.  These stubs replace the ecall-based versions
 * from libgloss so malloc works and any stray printf is silently discarded.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

/* ── heap: grow from _ebss upward ─────────────────────────────────────── */
extern char _ebss;
static char *heap_end = 0;

void *_sbrk(ptrdiff_t increment)
{
    if (heap_end == 0)
        heap_end = &_ebss;
    char *prev = heap_end;
    heap_end += increment;
    return (void *)prev;
}

/* ── I/O: discard all writes (no stdout on bare-metal — we use UART MMIO) */
int _write(int fd, const char *buf, int len) { (void)fd; (void)buf; return len; }
int _read (int fd, char *buf, int len)       { (void)fd; (void)buf; errno = ENOSYS; return -1; }
int _close(int fd)                           { (void)fd; return 0; }
int _fstat(int fd, struct stat *st)          { (void)fd; (void)st; errno = ENOSYS; return -1; }
int _isatty(int fd)                          { (void)fd; return 1; }
int _lseek(int fd, int off, int whence)      { (void)fd; (void)off; (void)whence; errno = ENOSYS; return -1; }
void _exit(int status)                       { (void)status; while (1); }

/* ── stdio no-ops: override libc printf/puts so the linker never pulls in   */
/* _vfprintf_r, __gdtoa, double-math, or the full stdio machinery.           */
/* NNoM uses printf/puts for internal error/stat messages only — on           */
/* bare-metal these are silently discarded; real output goes via uart_puts.  */
#include <stdarg.h>
int printf(const char *fmt, ...) { (void)fmt; return 0; }
int puts(const char *s)          { (void)s;   return 0; }
int putchar(int c)               { return c; }
