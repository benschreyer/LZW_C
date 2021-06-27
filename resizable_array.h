#ifndef RESIZABLE_ARRAY_H
    #define RESIZABLE_ARRAY_H

    // Code body for header file




    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h> 
    #include "resizable_array.c"

    struct array;

    void array_init(struct array* arr, unsigned int len, unsigned int ele_siz, int (*comp)(void* a, void* b));

    void array_set(struct array* arr, unsigned int index, void* set);

    void* array_get(struct array* arr, unsigned int index);

    void array_resize(struct array* arr, unsigned int new_siz);

    void print_int_array(struct array* arr);

    unsigned int binary_search(struct array* arr, void* key);

    unsigned int recursive_binary_search(struct array* arr, void* key, unsigned int lower, unsigned int upper);

#endif