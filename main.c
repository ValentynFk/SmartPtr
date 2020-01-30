#include <stdlib.h>
#include <stdio.h>
#include "smart-ptr.h"

void use_my_ptr(old_shared_ptr_t *shared_ptr) {
    printf("Raw ptr value is: %d\n", *((int *)old_shared_ptr_to_raw_ptr(shared_ptr)));
    old_shared_ptr_clean(&shared_ptr);
}

int main (int argh, char ** argv)
{
    int *raw_ptr = malloc(sizeof(int));
    *raw_ptr = 3;
    old_shared_ptr_t *shared_ptr = old_shared_ptr_from_raw_ptr(raw_ptr, free);
    raw_ptr = NULL;
    use_my_ptr(old_shared_ptr_copy(shared_ptr));
    old_shared_ptr_clean(&shared_ptr);
    /*
    printf("Raw ptr value is: %d\n", *((int *)shared_ptr_to_raw_ptr(shared_ptr1)));
    shared_ptr_t *shared_ptr2 = shared_ptr_copy(shared_ptr1);
    shared_ptr_clean(&shared_ptr1);
    printf("Raw ptr value is: %d\n", *((int *)shared_ptr_to_raw_ptr(shared_ptr2)));
    use_my_ptr(shared_ptr_copy(shared_ptr2));
    shared_ptr_clean(&shared_ptr2);
     */

    //shared_ptr_t *custom_shared_ptr = NULL;

    //(int *)shared_ptr_to_raw_ptr(custom_shared_ptr);
    //printf("Custom target is: %d\n", *((int *)shared_ptr_to_raw_ptr(custom_shared_ptr)));
    //shared_ptr_clean(&custom_shared_ptr);


    shared_ptr_t sh_ptr = pal_shared_ptr_alloc(sizeof(int));
    *(int*)sh_ptr.ptr = 1024;
    printf("The value of the shared pointer: %d\n", *(int*)sh_ptr.ptr);
    shared_ptr_t other_sh_ptr = pal_shared_ptr_copy(sh_ptr);
    pal_shared_ptr_free(&sh_ptr); // sh_ptr is now invalid
    weak_ptr_t weak_ptr = pal_weak_ptr_create(other_sh_ptr);
    printf("Weak pointer is dangling (y or n)? %c\n", pal_weak_ptr_dangling(weak_ptr)? 'y' : 'n');
    printf("The value of the shared pointer: %d\n", *(int*)other_sh_ptr.ptr);
    pal_shared_ptr_free(&other_sh_ptr); // sh_ptr is now invalid
    printf("Weak pointer is dangling (y or n)? %c\n", pal_weak_ptr_dangling(weak_ptr)? 'y' : 'n');

    return 0;
}
