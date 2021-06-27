#ifndef LZW_TREE_H
    #define LZW_TREE_H

    #include "resizable_array.h"
    #include "LZW_tree.c"

    struct LZW_node;

    void LZW_node_init(struct LZW_node* node, unsigned int c, unsigned int d);

    struct LZW_node* LZW_find_longest_match(struct LZW_node* root_node, struct array* byte_array);

#endif