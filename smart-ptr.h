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

// Shared & weak pointers operations
void *shared_ptr_alloc(size_t len,
        void *(*allocator)(const size_t),
        void (*deallocator)(void *));
void *shared_ptr_copy(void *ptr);
void *weak_ptr_create(void *ptr);
void weak_ptr_remove(void *ptr);
bool weak_ptr_dangling(void *ptr);
size_t shared_ptr_count(void *ptr);
size_t shared_ptr_wcount(void *ptr);
void shared_ptr_free(void *ptr);

// Default memory management using malloc and free
#define shared_ptr_default_alloc(len) (shared_ptr_alloc((len), malloc, free))

#endif // SMART_PTR_H
