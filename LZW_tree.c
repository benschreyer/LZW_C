

struct LZW_node
{
    struct array* leaves;
    unsigned int code;
    unsigned int depth;
};

void LZW_node_init(struct LZW_node* node, unsigned int c, unsigned int d)
{
    node->leaves = (struct array*) malloc(sizeof(struct array));
    array_init(node->leaves, 256, sizeof(struct LZW_node*), NULL);


    for(unsigned int i = 0;i < 256;i++)
    {
        struct LZW_node* nl = NULL;
        array_set(node->leaves, i, &nl);
        
    }

    node->code = c;

    node->depth = d;
}

struct LZW_node* LZW_find_longest_match(struct LZW_node* root_node, struct array* byte_array)
{
    struct LZW_node* curr = root_node;
    for(unsigned int i = 0;i < byte_array->length;i++)
    {
        //printf("\n\nTRYING BYTE INDEX FETCH\n");
        unsigned int byte_index = (unsigned int)  *((unsigned char*)array_get(byte_array, i));
        //printf("BYTE INDEX %u\n", byte_index);
        //printf("GETTING LEAVES\n");
        //printf("%p CURR %u %u %p\n",curr, curr->depth, curr->code, curr->leaves);
        struct LZW_node* next = *((struct LZW_node**) array_get(curr->leaves, byte_index) );
        //printf("%p NEXT\n",next);
        if( next != NULL)
        {
            //printf("CONT\n");
            curr = next;
        }
        else
        {
           // printf("RET\n");
            return curr;
        }
        
    }
    return curr;
}