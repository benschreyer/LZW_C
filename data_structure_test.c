#include "resizable_array.h"
#include "stdio.h"

int int_compare(void* a, void* b)
{
    return *((int *)a) - *((int *)b);
}

int main(int argc, char* argv[])
{
    struct array test_arr;
    array_init(&test_arr, 5, sizeof(int), &int_compare);

    int set = -3245;
    array_set(&test_arr, 0, &set);

    set = -323;
    array_set(&test_arr, 1, &set);

    set = 3;
    array_set(&test_arr, 2, &set);

    set = 32223;
    array_set(&test_arr, 3, &set);

    set = 52223;
    array_set(&test_arr, 4, &set);

    print_int_array(&test_arr);
    printf("\n\n\n");

    array_resize(&test_arr, 10);

    array_set(&test_arr, 5, &set);

    set = 72223;
    array_set(&test_arr, 6, &set);

    set = 72224;
    array_set(&test_arr, 7, &set);

    set = 82223;
    array_set(&test_arr, 8, &set);

    set = 90000000;
    array_set(&test_arr, 9, &set);

    print_int_array(&test_arr);
    printf("\n\n\n");

    int key = -3244;
    printf("%d location of 32223\n", binary_search(&test_arr, (void*) &key));


    return 0;
}