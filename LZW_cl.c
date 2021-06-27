#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h> 

#include "resizable_array.h"
#include "LZW_tree.h"

struct link;
struct linked_list;

struct link
{
    struct link* next;
    struct link* prev;
    struct byte_array* val;
};

struct linked_list
{
    struct link* start;
    struct link* end;
    unsigned int length;
};

struct byte_array
{
    char* data;
    unsigned int length;
};

struct byte_array* ba_init(unsigned int len)
{
    struct byte_array* ret = (struct byte_array*) malloc(sizeof(struct byte_array));
    ret->data = (char*)malloc(sizeof(char) * len);
    ret->length = len;
    return ret;
}


void ll_init(struct linked_list* list)
{
    list->length = 0;
    list->start = NULL;
    list->end = NULL;
}

void ll_append(struct linked_list* list, struct byte_array* val)
{
    printf("MALLOC 0\n");
    struct link* temp = (struct link*) malloc(sizeof(struct link));
    temp->prev = list->end;
    temp->next = (struct link*) NULL;
    temp->val = val;


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

void print_bytes(char* inp, unsigned int len)
{
    for(int i = 0;i < len;i++)
    {
        printf("%x\t",inp[i]);
    }
    printf("\n");
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

    if(argv[1][0] == 'c' )
    {
        printf("Compressing File Specified.\n");
        char ch;
        FILE* fp;

        FILE* wp;

        int alen = 0;
        while(argv[2][alen] != NULL)
        {
            alen++;
        }

        
        //printf("MALLOC 1\n");
        char* out_name = (char*) malloc((alen + 1 + 4) * sizeof(char));

        memcpy(out_name, argv[2], alen);

        out_name[alen] = '.';
        out_name[alen + 1] = 'b';
        out_name[alen + 2] = 'z';
        out_name[alen + 3] = 'p';
        out_name[alen + 4] = (char) NULL;


        wp = fopen(out_name, "w");

        struct LZW_node dictionary;
        LZW_node_init(&dictionary, 0, 0);
        unsigned int num_codes = 0;
        for(unsigned int i = 0; i < 256;i++)
        {
            struct LZW_node* temp = (struct LZW_node*) malloc(sizeof(struct LZW_node));

            LZW_node_init(temp, num_codes, 1);
            num_codes++;

            array_set(dictionary.leaves, i, &temp);

        }

        

        unsigned int longest_entry = 1;





        fp = fopen(argv[2],"r");
        if(fp == NULL)
        {
            //printf("TERMINATING: Please provide valid filenname to compress\n");
            exit(0);
        }



        //printf("%p address of file\n", fp);


        fseek(fp, 0L, SEEK_END);
        unsigned int file_size = ftell(fp);
        rewind(fp);

        unsigned int bytes_read = 0;

        //printf("%d FILE SIZE\n", file_size);
        int lc = 0;
        while(bytes_read < file_size)
        {
            bool skip_dict = false;
            //printf("====================%d\n",bytes_read);
            lc+=1;
            char* read = NULL;
            unsigned int read_length = 0;
            if(file_size - bytes_read >= longest_entry + 1)
            {
                read_length = longest_entry + 1;
                bytes_read += read_length;
                //printf("MALLOC 3\n");
                read = (char*) malloc(sizeof(char) * (read_length));
                fread(read, 1, read_length, fp);


            }
            else
            {
                read_length = file_size - bytes_read;
                bytes_read += read_length;
                printf("MALLOC 4\n");
                read = (char*) malloc(sizeof(char) * (read_length));
                fread(read, 1, read_length, fp);
                skip_dict = true;
            }
            //printf("[%s]\n READ IN\n",read);

            //printf("FETCH LONGEST STEP\n");

            struct array* read_as_byte_array = (struct array*) malloc(sizeof(struct array));

            //printf("FETCH LONGEST STEP2\n");
            read_as_byte_array->length = read_length;
            read_as_byte_array->element_size = sizeof(char);


            //printf("FETCH LONGEST STEP3\n");
            read_as_byte_array->data = read;


            //printf("FETCH LONGEST STEP4\n");
            struct LZW_node* node_longest_match = LZW_find_longest_match(&dictionary, read_as_byte_array);

            //printf("FETCH LONGEST STEP5 %p\n", node_longest_match);
            unsigned int longest_match = node_longest_match->depth;

            unsigned int match_index = node_longest_match->code;
            
            if(match_index < 128)
            {
                //printf("SINGLE BYTE %x HEX OF SINGLE WIDTH %d VAL OF GRTST BIT\n",(char)(match_index & 0x7F), (unsigned int)(char)(match_index & 0x7F) & 0x80);
                char write_char = (char)(match_index & 0x7F);
                fwrite(&write_char, 1, 1, wp);
            }
            else
            {
                unsigned int bytes_needed = 0;
                for(int i = 0;i < 4;i++)
                {
                    if( (0xFF << (8*(3 - i))) & match_index)
                    {
                        //printf("%d break on\n", i);
                        bytes_needed = (4 - i);
                        break;
                    }
                }
                
                char write_char = ((char) bytes_needed) | 0x80;
                fwrite(&write_char, 1, 1, wp);
                //printf("MATCH IND HEX %x\t", match_index);
                //match_index = match_index << (8 * (4 - bytes_needed));
                fwrite(&match_index, 1, bytes_needed, wp);
                //printf("MULTI BYTE\t%x written, %u\tWRITE CHAR %x\n",match_index, bytes_needed, 0xFF & (unsigned int)write_char);
                //printf("%u MULTI BYTE %u \t %u HEX OF SINGLE WIDTH %d VAL OF GRTST BIT\n",bytes_needed,match_index,0xFF & (unsigned int)(0x80 ^ (char)(bytes_needed)), 0xFF & (unsigned int)(write_char) & 0x80);
            
            }


            //printf("%u cutoff!\n", cutoff);
            //fwrite(longest_match_buf, 1, longest_match, wp);
            //fwrite(read + longest_match, 1, read_length - longest_match, wp);
            for(unsigned int i = 0; i < read_length - longest_match;i++)
            {
                bytes_read--;
                ungetc(read[read_length - i - 1], fp);
                //printf("[%c] UNUSED\n", read[read_length - i - 1]);
            }
            //printf("DICT STEP\n");
            if(!skip_dict && 3000000 > num_codes)
            {
                if(node_longest_match->depth + 1 > longest_entry)
                {
                    longest_entry = node_longest_match->depth + 1;
                }

                struct LZW_node* temp_add = (struct LZW_node*) malloc(sizeof(struct LZW_node));

                LZW_node_init(temp_add, num_codes, node_longest_match->depth + 1);
                num_codes++;

                //printf("NEW SET %p %u %u\n\n", temp_add, temp_add->code, temp_add-> depth);

                //printf("NEW SET INDEX %u\n",(unsigned char)read[longest_match]);

                array_set(node_longest_match->leaves, (unsigned int)(unsigned char)read[longest_match], &temp_add);

                //free(temp_add);
                
            }

            //printf("ADD\n%s \nADD\n %d\n",to_add, 1 +longest_match);
            free(read_as_byte_array->data);
            //free(read_as_byte_array);
            free(read_as_byte_array);
            //printf("Total Read %d \t %d\n====================\n",bytes_read,file_size);
        }


        

        fclose(wp);
        fclose(fp);
        return 0;
    }
    else
    {

        int alen = 0;
        while(argv[2][alen] != NULL)
        {
            alen++;
        }

        printf("%d\n",argc);
        char ch;
        FILE* fp;

        FILE* wp;

        printf("MALLOC 6\n");
        char* out_name = (char*) malloc(sizeof(char) * (alen - 4 + 1));

        memcpy(out_name, argv[2], alen - 4);
        out_name[alen - 4] = (char) NULL;

        wp = fopen(out_name, "w");

        fp = fopen(argv[2],"r");
        if(fp == NULL)
        {
            printf("TERMINATING: Please provide valid filenname to compress\n");
            exit(0);
        }

        fseek(fp, 0L, SEEK_END);
        unsigned int file_size = ftell(fp);
        rewind(fp);

        /*struct LZW_node dictionary;
        LZW_node_init(&dictionary, 0, 0);
        unsigned int num_codes = 0;
        for(unsigned int i = 0; i < 256;i++)
        {
            struct LZW_node* temp = (struct LZW_node*) malloc(sizeof(struct LZW_node));

            LZW_node_init(temp, num_codes, 1);
            num_codes++;

            array_set(dictionary.leaves, i, &temp);

        }*/
        struct array* dictionary = (struct array*) malloc(sizeof(struct array));

        array_init(dictionary, 256, sizeof(struct array*), NULL);

        unsigned int num_codes = 0;

        for(unsigned int i = 0;i < 256;i++)
        {
            struct array* temp = malloc(sizeof(struct array));
            array_init(temp, 1, sizeof(char),NULL);
            char set = (char)i;
            array_set(temp, 0, &set);
            array_set(dictionary, i, &temp);
            num_codes++;
        }

        unsigned int longest_entry = 1;
        int co = 0;
        unsigned int bytes_read = 0;
        //printf("=======================\n");
        while(bytes_read < file_size)
        {
            co++;
            //printf("=========================\n");

            char leading_byte = 0;

            unsigned int index = 0;
            fread(&leading_byte, 1, 1, fp);
            //printf("%x LEADING BYTE\n",0xFF & (unsigned int)leading_byte);
            if((leading_byte & 0x80) == 0)
            {
                index = 0xFF & (unsigned int)leading_byte;
                bytes_read+=1;
            }
            else
            {
                unsigned int bytes_needed = 0xFF & (unsigned int) (leading_byte ^ 0x80);
                bytes_read+=bytes_needed + 1;
                fread(&index, bytes_needed, 1, fp);
            }

            

            //printf("%d %d\n", index, co);


            //printf("%p \n", iter);
            //printf("%s %d\n", iter->val,index);
            //printf("HERE1\n");
            struct array* get_index = *((struct array**)array_get(dictionary, index));
            //printf("HERE2 %p\n",get_index);
            fwrite(get_index->data, get_index->element_size, get_index->length,wp);
            //printf("HERE3\n");
            index = 0;
            leading_byte = 0;
            fread(&leading_byte, 1, 1, fp);
            //printf("%x LEADING BYTE\n",0xFF & (unsigned int)leading_byte);

            unsigned int bytes_used = 0;
            if((leading_byte & 0x80) == 0)
            {
                index = 0xFF & (unsigned int)leading_byte;
            }
            else
            {
                unsigned int bytes_needed = 0xFF & (unsigned int) (leading_byte ^ 0x80);
                bytes_used=bytes_needed;
                //printf("%d bytes needeed multi\n",bytes_needed);
                fread(&index, bytes_needed, 1, fp);
            }
            
            for(int i = bytes_used - 1; i >= 0;i--)
            {
                //printf("%x HEX UNGET\n", (char)0xFF && (index >> (8 * i)),fp);
                ungetc((char)0xFF & (index >> (8 * i)),fp);
            }
            ungetc(leading_byte, fp);

            //printf("MALLOC 8\n");

            
            while(num_codes >= dictionary->length)
            {
                array_resize(dictionary, dictionary->length * 2);
                
            }

            struct array* last_char_source = NULL;
            char last_char = 0;
            if(index >= num_codes)
            {
                last_char_source = get_index;
            }
            else
            {
               last_char_source = *((struct array**)array_get(dictionary, index));
            }

            last_char = *((char*)array_get(last_char_source, 0));

            struct array* to_add = (struct array*) malloc(sizeof(struct array));
            array_init(to_add, 1 + get_index->length,sizeof(char), NULL);
            memcpy(to_add->data, get_index->data, get_index->length);
            array_set(to_add, get_index->length, &last_char);
            array_set(dictionary, num_codes, &to_add);
            



            
            num_codes++;

            /*struct byte_array* add_data = ba_init(iter->val->length + 1);

            memcpy(add_data->data, iter->val->data, iter->val->length);

            struct link* iter2 = dictionary.start;
            for(unsigned int i = 0; i < index;i++)
            {
                iter2 = iter2->next;
            }



            if(index >= dictionary.length)
            {
                add_data->data[iter->val->length] = add_data->data[0];
            }
            else
            {
                add_data->data[iter->val->length] = iter2->val->data[0];
            }
            
            
            if(!feof(fp))
            {
                ll_append(&dictionary, add_data);
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
            //printf("=========================\n");*/
        }
        fclose(fp);
        fclose(wp);
    }

    
}