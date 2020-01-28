#include <stdlib.h>
#include "smart-ptr.h"

// Shared pointer initialization
shared_ptr_t *shared_ptr_from_raw_ptr(void *raw_ptr, void (*dealloc_handler)(void *))
{
    if (raw_ptr == NULL)
    {
        return NULL;
    }
    shared_ptr_t *shared_ptr = malloc(sizeof(shared_ptr_t)); // Construct shared ptr
    shared_ptr -> target = raw_ptr;                          // Construct target
    shared_ptr -> cb = malloc(sizeof(control_block_t));      // Construct CB
    shared_ptr -> cb -> ref_count = 1;
    shared_ptr -> cb -> weak_count = 0;
    shared_ptr -> cb -> dealloc_handler = dealloc_handler;
    return shared_ptr;
}

// Shared pointer deinitialization
int shared_ptr_clean(shared_ptr_t **shared_ptr)
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

