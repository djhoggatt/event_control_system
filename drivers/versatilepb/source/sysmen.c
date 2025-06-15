#include <errno.h>
#include <sys/types.h>

extern char _heap_start; // Defined in linker script 
extern char _heap_end;   // Defined in linker script

static char *heap_end = &_heap_start;

void *_sbrk(ptrdiff_t incr)
{
    char *prev_heap_end = heap_end;

    if ((heap_end + incr) > &_heap_end)
    {
        errno = ENOMEM;
        return (void *)-1;
    }

    heap_end += incr;
    return prev_heap_end;
}