#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "smart-ptr.h"

void use_my_ptr(shared_ptr_t *shared_ptr) {
    printf("Raw ptr value is: %d\n", *((int *)shared_ptr_to_raw_ptr(shared_ptr)));
    shared_ptr_clean(&shared_ptr);
}

int main (int argh, char ** argv)
{
    int *raw_ptr = malloc(sizeof(int));
    *raw_ptr = 3;
    shared_ptr_t *shared_ptr1 = shared_ptr_from_raw_ptr(raw_ptr, free);
    raw_ptr = NULL;

    printf("Raw ptr value is: %d\n", *((int *)shared_ptr_to_raw_ptr(shared_ptr1)));
    shared_ptr_t *shared_ptr2 = shared_ptr_copy(shared_ptr1);
    shared_ptr_clean(&shared_ptr1);
    printf("Raw ptr value is: %d\n", *((int *)shared_ptr_to_raw_ptr(shared_ptr2)));
    use_my_ptr(shared_ptr_copy(shared_ptr2));
    shared_ptr_clean(&shared_ptr2);
    
    shared_ptr_t *custom_shared_ptr = NULL;

    (int *)shared_ptr_to_raw_ptr(custom_shared_ptr);
    //printf("Custom target is: %d\n", *((int *)shared_ptr_to_raw_ptr(custom_shared_ptr)));
    shared_ptr_clean(&custom_shared_ptr);

    return 0;
}
