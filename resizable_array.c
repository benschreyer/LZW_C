//Benjamin Schreyer Resizable Array 6/2021

struct array
{
    //Byte width of one array element
    unsigned int element_size;
    //Number of elements in the array
    unsigned int length;
    //Compare function, taking pointers so that the array can have any type
    int (*compare)(void* a, void* b);
    
    //Pointer to data
    void* data;
};

//Free memory used by array
void array_free(struct array* arr)
{
    free(arr->data);
    free(arr);
}

void array_init(struct array* arr, unsigned int len, unsigned int ele_siz, int (*comp)(void* a, void* b))
{
    //Assign properties and allocate memory
    arr->length = len;
    arr->element_size = ele_siz;
    arr->data = malloc(arr->length * ele_siz);
    arr->compare = comp;
}

void array_set(struct array* arr, unsigned int index, void* set)
{
    //Copy element_size bytes into the array from the pointer set
    memcpy((char*)arr->data + (arr->element_size) * index, (char*) set, arr->element_size);
}

void* array_get(struct array* arr, unsigned int index)
{
    //Return a pointer to the index'th element
    return (void*)(((char*) arr->data) + index * arr->element_size);
}

void array_resize(struct array* arr, unsigned int new_siz)
{
    //attempt to resize buffer, if it returns NULL(failure), allocate new memory and copy contents free old buffer
    void* temp;
    if(!(temp = realloc(arr->data, arr->element_size * new_siz)))
    {
        temp = malloc(arr->element_size * new_siz);
        memcpy((char*)temp, (char*)arr->data ,arr->length * arr->element_size);
        free(arr->data);
    }
    
    arr->data = temp;
    arr->length = new_siz;
}

//Binary search using compare function pointer, not validated, user must validate whether key was found manually
unsigned int recursive_binary_search(struct array* arr, void* key, unsigned int lower, unsigned int upper)
{
    printf("LOWER %u\tUPPER %u\n",lower, upper);
    if(lower >= upper)
    {
        return upper;
    }

    unsigned int mid = lower + (upper - lower) / 2;

    int compare_result = (*arr->compare)(key, array_get(arr, mid));

    if(compare_result == 0)
    {
        return mid;
    }
    
    if(compare_result > 0)
    {
        return recursive_binary_search(arr, key, mid + 1, upper);
    }
    else
    {
        if(mid == 0)
        {
            return mid;
        }
        return recursive_binary_search(arr, key, lower, mid - 1);
    }


}

//Wrapper for nicer call
unsigned int binary_search(struct array* arr,void* key)
{
    return recursive_binary_search(arr, key, 0, arr->length - 1);
}

void print_int_array(struct array* arr)
{
    for(int i = 0; i < arr->length;i++)
    {
        printf("%d\t",*(int*)array_get(arr, i));
    }
}

/*
int main(int argc, char* argv[])
{
    struct array test_arr;
    array_init(&test_arr, 5, sizeof(int));

    int set = -3245;
    array_set(&test_arr, 0, &set);

    set = 323;
    array_set(&test_arr, 1, &set);

    set = -3;
    array_set(&test_arr, 2, &set);

    set = 3223;
    array_set(&test_arr, 3, &set);

    set = 1000;
    array_set(&test_arr, 4, &set);

    print_int_array(&test_arr);
    printf("\n\n\n");

    array_resize(&test_arr, 10);

    array_set(&test_arr, 5, &set);

    set = 5;
    array_set(&test_arr, 6, &set);

    set = 4;
    array_set(&test_arr, 7, &set);

    set = 3;
    array_set(&test_arr, 8, &set);

    set = 2;
    array_set(&test_arr, 9, &set);

    print_int_array(&test_arr);
    printf("\n\n\n");


    return 0;
}*/
