#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>
#include "smart-ptr.h"

#define GET_PTR_REF(type, ptr) (*((type *)(ptr)))
// TODO deveop solution for differentiating weak ptrs from shared ptrs
// Shared pointer from scratch
void *shared_ptr_alloc(const size_t len,
        void *(*allocator)(const size_t),
        void (*deallocator)(void *))
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
void *shared_ptr_copy(void *ptr)
{
    if (ptr == NULL) {
        return NULL;
    }
    // Increase the reference count
    GET_PTR_REF(size_t, ptr - sizeof(size_t)) += 1;
    return ptr;
}

void *weak_ptr_create(void *ptr)
{
    if (ptr == NULL) {
        return NULL;
    }
    // Increase the weak reference count
    GET_PTR_REF(size_t, ptr - sizeof(size_t) - sizeof(size_t)) += 1;
    return ptr;
}

void weak_ptr_remove(void *ptr)
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
    void (*deallocator)(void *) = *((void **)(ptr - sizeof(size_t) - sizeof(size_t) - sizeof(void *)));
    deallocator(ptr - sizeof(size_t) - sizeof(size_t) - sizeof(void *));
}

bool weak_ptr_dangling(void *ptr)
{
    if (ptr == NULL) {
        return true;
    }
    return !GET_PTR_REF(size_t, ptr - sizeof(size_t));
}

// Shared pointer reference count
size_t shared_ptr_count(void *ptr)
{
    if (ptr == NULL) {
        return 0;
    }
    return GET_PTR_REF(size_t, ptr - sizeof(size_t));
}

// Shared pointer weak reference count
size_t shared_ptr_wcount(void *ptr)
{
    if (ptr == NULL) {
        return 0;
    }
    return GET_PTR_REF(size_t, ptr - sizeof(size_t) - sizeof(size_t));
}

void shared_ptr_free(void *ptr)
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
    void (*deallocator)(void *) = *((void **)(ptr - sizeof(size_t) - sizeof(size_t) - sizeof(void *)));
    deallocator(ptr - sizeof(size_t) - sizeof(size_t) - sizeof(void *));
}

// Shared pointer initialization
old_shared_ptr_t *old_shared_ptr_from_raw_ptr(void *raw_ptr, void (*dealloc_handler)(void *))
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

// Shared pointer copying
shared_ptr_t *shared_ptr_copy(shared_ptr_t *shared_ptr)
{
    if (shared_ptr == NULL)
    {
        return NULL;
    }
    shared_ptr_t *copy = malloc(sizeof(shared_ptr_t));
    copy -> target = shared_ptr -> target;
    copy -> cb = shared_ptr -> cb;
    ++(shared_ptr -> cb -> ref_count);
    return copy;
}

// Shared pointer degradation
void *shared_ptr_to_raw_ptr(shared_ptr_t *shared_ptr)
{
    if (shared_ptr == NULL)
    {
        return NULL;
    }
    return shared_ptr -> target;
}

