#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>
#include "smart-ptr.h"



#define GET_PTR_REF(type, ptr) (*((type *)(ptr)))
// Shared pointer from scratch
static void *shared_ptr_alloc(const size_t len,
        alloc_handler_t allocator,
        dealloc_handler_t deallocator)
{
    // Create the shared pointer using the allocator and deallocator of the object
    if (allocator == NULL || deallocator == NULL || !len) {
        return NULL;
    }
    // Allocate the memory for the CB
    // [deallocator][weak_count][ref_count][object]
    //                                        ^(shared_ptr)
    void *cb = allocator(sizeof(void *) + sizeof(size_t) + sizeof(size_t) + len);
    *(void **)cb = deallocator;
    GET_PTR_REF(size_t, cb + sizeof(void *)) = 0;
    GET_PTR_REF(size_t, cb + sizeof(void *) + sizeof(size_t)) = 1;
    return cb + sizeof(void *) + sizeof(size_t) + sizeof(size_t);
}

// Shared pointer copying
static void *shared_ptr_copy(void *ptr)
{
    if (ptr == NULL) {
        return NULL;
    }
    // Increase the reference count
    GET_PTR_REF(size_t, ptr - sizeof(size_t)) += 1;
    return ptr;
}

static void *weak_ptr_create(void *ptr)
{
    if (ptr == NULL) {
        return NULL;
    }
    // Increase the weak reference count
    GET_PTR_REF(size_t, ptr - sizeof(size_t) - sizeof(size_t)) += 1;
    return ptr;
}

static void weak_ptr_remove(void *ptr)
{
   if (ptr == NULL) {
       return;
   }
   if (GET_PTR_REF(size_t, ptr - sizeof(size_t)) > 0 ||
       GET_PTR_REF(size_t, ptr - sizeof(size_t) - sizeof(size_t)) > 0) {
        // Other references exists
        if (GET_PTR_REF(size_t, ptr - sizeof(size_t) - sizeof(size_t)) != 0) {
            GET_PTR_REF(size_t, ptr - sizeof(size_t) - sizeof(size_t)) -= 1;
        }
        return;
    }
   // No references left
    dealloc_handler_t deallocator = *((void **)(ptr - sizeof(size_t) - sizeof(size_t) - sizeof(void *)));
    deallocator(ptr - sizeof(size_t) - sizeof(size_t) - sizeof(void *));
}

static bool weak_ptr_dangling(void *ptr)
{
    if (ptr == NULL) {
        return true;
    }
    return !GET_PTR_REF(size_t, ptr - sizeof(size_t));
}

// Shared pointer reference count
static size_t shared_ptr_count(void *ptr)
{
    if (ptr == NULL) {
        return 0;
    }
    return GET_PTR_REF(size_t, ptr - sizeof(size_t));
}

// Shared pointer weak reference count
static size_t shared_ptr_wcount(void *ptr)
{
    if (ptr == NULL) {
        return 0;
    }
    return GET_PTR_REF(size_t, ptr - sizeof(size_t) - sizeof(size_t));
}

static void shared_ptr_free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }
    if (GET_PTR_REF(size_t, ptr - sizeof(size_t)) > 1) {
        // Other reference exists
        GET_PTR_REF(size_t, ptr - sizeof(size_t)) -= 1;
        return;
    }
    if (GET_PTR_REF(size_t, ptr - sizeof(size_t) - sizeof(size_t)) > 0) {
        // Other weak reference exists
        if (GET_PTR_REF(size_t, ptr - sizeof(size_t)) != 0) {
            GET_PTR_REF(size_t, ptr - sizeof(size_t)) -= 1;
        }
        return;
    }
    // No references left
    dealloc_handler_t deallocator = *((void **)(ptr - sizeof(size_t) - sizeof(size_t) - sizeof(void *)));
    deallocator(ptr - sizeof(size_t) - sizeof(size_t) - sizeof(void *));
}

/* Pointers Abstraction Layer */
shared_ptr_t pal_shared_ptr_alloc(const size_t len)
{
    shared_ptr_t sh_ptr;
    sh_ptr.ptr = shared_ptr_alloc(len, malloc, free);
    return sh_ptr;
}
shared_ptr_t pal_shared_ptr_copy(shared_ptr_t sh_ptr)
{
    shared_ptr_t sh_copy;
    sh_copy.ptr = shared_ptr_copy(sh_ptr.ptr);
    return sh_copy;
}
size_t pal_shared_ptr_count(shared_ptr_t sh_ptr)
{
    return shared_ptr_count(sh_ptr.ptr);
}
size_t pal_shared_ptr_wcount(shared_ptr_t sh_ptr)
{
    return shared_ptr_wcount(sh_ptr.ptr);
}
void pal_shared_ptr_free(shared_ptr_t *sh_ptr)
{
    if (sh_ptr -> ptr != NULL) {
        shared_ptr_free(sh_ptr -> ptr);
        sh_ptr -> ptr = NULL;
    }
}

weak_ptr_t pal_weak_ptr_create(shared_ptr_t sh_ptr)
{
    weak_ptr_t w_ptr;
    w_ptr.ptr = weak_ptr_create(sh_ptr.ptr);
    return w_ptr;
}

bool pal_weak_ptr_dangling(weak_ptr_t w_ptr)
{
    return weak_ptr_dangling(w_ptr.ptr);
}

void pal_weak_ptr_remove(weak_ptr_t *w_ptr)
{
    if (w_ptr -> ptr != NULL) {
        weak_ptr_remove(w_ptr -> ptr);
        w_ptr -> ptr = NULL;
    }
}
// Shared pointer initialization
old_shared_ptr_t *old_shared_ptr_from_raw_ptr(void *raw_ptr, dealloc_handler_t dealloc_handler)
{
    if (raw_ptr == NULL)
    {
        return NULL;
    }
    old_shared_ptr_t *shared_ptr = malloc(sizeof(old_shared_ptr_t)); // Construct shared ptr
    shared_ptr -> target = raw_ptr;                          // Construct target
    shared_ptr -> cb = malloc(sizeof(old_control_block_t));      // Construct CB
    shared_ptr -> cb -> ref_count = 1;
    shared_ptr -> cb -> weak_count = 0;
    shared_ptr -> cb -> dealloc_handler = dealloc_handler;
    return shared_ptr;
}

// Shared pointer deinitialization
int old_shared_ptr_clean(old_shared_ptr_t **shared_ptr)
{
    if (shared_ptr == NULL || *shared_ptr == NULL)
    {
        return -1;
    }
    if ((*shared_ptr) -> cb -> ref_count > 1)
    {
        --((*shared_ptr) -> cb -> ref_count);
        free(*shared_ptr);                                               // Free the shared_ptr
        *shared_ptr = NULL; // Remove access to the shared ptr
        return 0;
    }
    (*shared_ptr) -> cb -> dealloc_handler((*shared_ptr) -> target); // Free the target
    free((*shared_ptr) -> cb);                                       // Free the CB
    free(*shared_ptr);                                               // Free the shared_ptr
    *shared_ptr = NULL; // Remove access to the shared ptr
    return 0;
}

// Shared pointer copying
old_shared_ptr_t *old_shared_ptr_copy(old_shared_ptr_t *shared_ptr)
{
    if (shared_ptr == NULL)
    {
        return NULL;
    }
    old_shared_ptr_t *copy = malloc(sizeof(old_shared_ptr_t));
    copy -> target = shared_ptr -> target;
    copy -> cb = shared_ptr -> cb;
    ++(shared_ptr -> cb -> ref_count);
    return copy;
}
// Shared pointer degradation
void *old_shared_ptr_to_raw_ptr(old_shared_ptr_t *shared_ptr)
{
    if (shared_ptr == NULL)
    {
        return NULL;
    }
    return shared_ptr -> target;
}

