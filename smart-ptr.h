#ifndef SMART_PTR_H
#define SMART_PTR_H

#include <stdio.h>

struct control_block_t {
    int ref_count;
    int weak_count;
    void (*dealloc_handler)(void *);
} typedef control_block_t;

struct shared_ptr_t {
    void *target;
    control_block_t *cb;
} typedef shared_ptr_t;

shared_ptr_t *shared_ptr_from_raw_ptr(void *raw_ptr, void (*dealloc_handler)(void *));
int shared_ptr_clean(shared_ptr_t **shared_ptr);
shared_ptr_t *shared_ptr_copy(shared_ptr_t *shared_ptr);
void *shared_ptr_to_raw_ptr(shared_ptr_t *shared_ptr);

//#define shared_ptr_from_raw_ptr(arg) (shared_ptr_from_raw_ptr((arg), free))

#endif // SMART_PTR_H
