#ifndef SMART_PTR_H
#define SMART_PTR_H

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>

typedef void (*dealloc_handler_t)(void *);
typedef void *(*alloc_handler_t)(const size_t);

/* deprecated */
typedef struct old_control_block_t {
    int ref_count;
    int weak_count;
    void (*dealloc_handler)(void *);
} old_control_block_t;

typedef struct old_shared_ptr_t {
    void *target;
    old_control_block_t *cb;
} old_shared_ptr_t;

old_shared_ptr_t *old_shared_ptr_from_raw_ptr(void *raw_ptr, dealloc_handler_t dealloc_handler);
int old_shared_ptr_clean(old_shared_ptr_t **shared_ptr);
old_shared_ptr_t *old_shared_ptr_copy(old_shared_ptr_t *shared_ptr);
void *old_shared_ptr_to_raw_ptr(old_shared_ptr_t *shared_ptr);
/* end deprecated */

typedef struct shared_ptr_t {
    void *ptr;
} shared_ptr_t;

typedef struct weak_ptr_t {
    void *ptr;
} weak_ptr_t;

// Shared & weak pointers operations
shared_ptr_t pal_shared_ptr_alloc(size_t);
shared_ptr_t pal_shared_ptr_copy(shared_ptr_t);
size_t       pal_shared_ptr_count(shared_ptr_t);
size_t       pal_shared_ptr_wcount(shared_ptr_t);
void         pal_shared_ptr_free(shared_ptr_t *);
weak_ptr_t   pal_weak_ptr_create(shared_ptr_t);
bool         pal_weak_ptr_dangling(weak_ptr_t);
void         pal_weak_ptr_remove(weak_ptr_t *);

// Default memory management using malloc and free
#define shared_ptr_default_alloc(len) (shared_ptr_alloc((len), malloc, free))

#endif // SMART_PTR_H
