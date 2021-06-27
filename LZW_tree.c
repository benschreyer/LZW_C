#include "resizable_array.h"

struct LZW_node
{
    struct array* leaves;
    unsigned int code;
}

void LZW_node_init(struct LZW_node* node, unsigned int c)
{
    node->leaves = (struct array*) malloc(sizeof(struct array));
    array_init(leaves, 256, sizeof(struct LZW_node*), NULL);

    memset(leaves->data, (int) NULL, leaves->element_size * leaves ->length);

    node->code = c;
}

struct LZW_node* LZW_find_longest_match(struct LZW_node* root_node, struct array* byte_array)
{
    struct LZW_node* curr = root_node;
    for(unsigned int i = 0;i < byte_array->length;i++)
    {
        struct LZW_node* next = *((struct LZW_node**) array_get(curr->leaves, (unsigned int)  *((unsigned char*)array_get(byte_array, i)) ));
        if( next != NULL)
        {
            curr = next;
        }
        else
        {
            return curr;
        }
    }
}