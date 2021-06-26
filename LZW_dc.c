#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h> 

struct link;
struct linked_list;

struct link
{
    struct link* next;
    struct link* prev;
    char* val;
    unsigned int length;
};

struct linked_list
{
    struct link* start;
    struct link* end;
    unsigned int length;
};



void ll_init(struct linked_list* list)
{
    list->length = 0;
    list->start = NULL;
    list->end = NULL;
}

void ll_append(struct linked_list* list, char* val, unsigned int len)
{

    struct link* temp = (struct link*) malloc(sizeof(struct link));
    temp->prev = list->end;
    temp->next = NULL;
    temp->val = val;
    temp->length = len;

    if(list->start == NULL || list->length == 0)
    {
        printf("%s  START SET\n", val);
        list->start = temp;
        list->end = temp;
    }
    list->length++;
    list->end->next = temp;
    list->end = temp;
}

int main( int argc, char *argv[] )  
{

    /*struct linked_list test_list;

    ll_init(&test_list);

    char my_string1[11] = "abcdefghij";
    ll_append(&test_list,&my_string1[0]);
    char my_string2[11] = "a32defghij";
    ll_append(&test_list,&my_string2[0]);
    char my_string3[11] = "AAAdefghij";
    ll_append(&test_list,&my_string3[0]);

    printf("%s\n\n",test_list.start->val);

    printf("%d items in list\n", test_list.length);*/



    printf("%d\n",argc);
    char ch;
    FILE* fp;

    FILE* wp;
    wp = fopen(argv[1], "w");

    fp = fopen("out.bin","r");
    if(fp == NULL)
    {
        printf("TERMINATING: Please provide valid filenname to compress\n");
        exit(0);
    }

    fseek(fp, 0L, SEEK_END);
    unsigned int file_size = ftell(fp);
    rewind(fp);

    struct linked_list dictionary;
    ll_init(&dictionary);

    for(unsigned int i = 0; i < 256;i++)
    {
        char* temp_add = malloc(sizeof(char) * 2);
        temp_add[0] = (char) i;
        temp_add[1] = (char) NULL;
        ll_append(&dictionary, temp_add, 1);
    }

    

    unsigned int longest_entry = 1;
    int co = 0;
    unsigned int bytes_read = 0;
    printf("=======================\n");
    while(bytes_read < file_size)
    {
        co++;
        //printf("=========================\n");
        unsigned int index = 0;
        fread(&index, 4, 1, fp);
        bytes_read+=4;

        printf("%d %d\n", index, co);

        struct link* iter = dictionary.start;

        
        for(unsigned int i = 0; i < index;i++)
        {
            iter = iter->next;
        }
        //printf("%p \n", iter);
        //printf("%s %d\n", iter->val,index);
        fwrite(iter->val, 1, iter->length,wp);
        index = 0;
        fread(&index, 4, 1, fp);

        for(int i = 3; i >= 0;i--)
        {
            //printf("%x HEX UNGET\n", (char)0xFF && (index >> (8 * i)),fp);
            ungetc((char)0xFF & (index >> (8 * i)),fp);
        }

        char* add_str = (char*) malloc(sizeof(char) * (iter->length + 2));
        memcpy(add_str, iter->val, iter->length);

        struct link* iter2 = dictionary.start;
        for(unsigned int i = 0; i < index;i++)
        {
            iter2 = iter2->next;
        }

        add_str[iter->length + 1] = NULL;

        if(index >= dictionary.length)
        {
            add_str[iter->length] = add_str[0];
        }
        else
        {
            add_str[iter->length] = iter2->val[0];
        }
        
        
        if(!feof(fp))
        {
            ll_append(&dictionary, add_str,iter->length + 1);
        }
        //struct link* it = dictionary.start;

        /*int co = 0;
        while(it != NULL)
        {
            if(co > 254)
            {
                //printf("%s\n",it->val);
            }
            co+=1;
            it = it->next;
        }*/
        //printf("=========================\n");
    }
    fclose(fp);
    fclose(wp);
}