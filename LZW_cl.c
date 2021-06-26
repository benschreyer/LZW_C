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

        

        char* out_name = (char*) malloc((alen + 1 + 4) * sizeof(char));

        memcpy(out_name, argv[2], alen);

        out_name[alen] = '.';
        out_name[alen + 1] = 'b';
        out_name[alen + 2] = 'z';
        out_name[alen + 3] = 'p';
        out_name[alen + 4] = (char) NULL;


        wp = fopen(out_name, "w");

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





        fp = fopen(argv[2],"r");
        if(fp == NULL)
        {
            printf("TERMINATING: Please provide valid filenname to compress\n");
            exit(0);
        }



        printf("%p address of file\n", fp);


        fseek(fp, 0L, SEEK_END);
        unsigned int file_size = ftell(fp);
        rewind(fp);

        unsigned int bytes_read = 0;

        printf("%d FILE SIZE\n", file_size);
        int lc = 0;
        while(bytes_read < file_size)
        {
            bool skip_dict = false;
            printf("====================\n");
            lc+=1;
            char* read = NULL;
            unsigned int read_length = 0;
            if(file_size - bytes_read >= longest_entry + 1)
            {
                read_length = longest_entry + 1;
                bytes_read += read_length;
                
                read = (char*) malloc(sizeof(char) * (read_length + 1));
                fread(read, 1, read_length, fp);
                printf("READ %d\n", read_length);
                read[read_length] = (char) NULL ;

            }
            else
            {
                read_length = file_size - bytes_read;
                bytes_read += read_length;
                
                read = (char*) malloc(sizeof(char) * (read_length + 1));
                fread(read, 1, read_length, fp);
                read[read_length] = (char) NULL;
                skip_dict = true;
            }
            printf("[%s]\n READ IN\n",read);
            struct link* iter = dictionary.start;
            
            unsigned int longest_match = 0;
            char* longest_match_buf = NULL;
            //printf("%s dic start next\n",iter->val);
            unsigned int match_index = 0;
            unsigned int ik = 0;
            while(iter != NULL)
            {   
                
                unsigned int match_len = 0;
                bool iter_cont = false;
                for(int i = 0; i < read_length && i < iter->length;i++)
                {
                    if(iter->val[i] != read[i])
                    {
                        iter_cont = true;
                        break;
                    }
                    else
                    {
                        match_len++;
                    }
                }

                if(!iter_cont)
                {
                    if(match_len > longest_match)
                    {
                        longest_match = match_len;
                        longest_match_buf = iter->val;
                        match_index = ik;
                    }

                }
                //printf("LL ITER %d %p\n", lc, iter->next);
                iter = iter->next;
                ik++;
            }
            printf("%s longest match ptr\n",longest_match_buf);
            //printf("%d sizeof uint\n", sizeof(unsigned int));
            unsigned int cutoff = 0;
            /*for(unsigned int i = 0;i < 32;i++)
            {
                if((match_index << i) &(0b10000000000000000000000000000000))
                {
                    cutoff = i;
                    break;
                }
            }

            unsigned int nbits = (32 - cutoff);
            unsigned int nbytes = nbits/8;
            if(nbits % 8 != 0)
            {
                nbytes += 1;
            }



            fputc((char) nbytes,wp);*/
            printf("%d MATHC IND\n", match_index);
            fwrite(&match_index, 4, 1, wp);
            //printf("%u cutoff!\n", cutoff);
            //fwrite(longest_match_buf, 1, longest_match, wp);
            //fwrite(read + longest_match, 1, read_length - longest_match, wp);
            for(unsigned int i = 0; i < read_length - longest_match;i++)
            {
                bytes_read--;
                ungetc(read[read_length - i - 1], fp);
                printf("[%c] UNUSED\n", read[read_length - i - 1]);
            }
            if(!skip_dict)
            {

                char* to_add = (char *) malloc(sizeof(char) * (2 + longest_match));
                memcpy(to_add, read, 1 + longest_match);
                to_add[2 + longest_match] = (char) NULL;

                //Seek back unused input
                //fseek(fp, -1*(read_length - longest_match), SEEK_CUR);

                //to_add[2 + longest_match] = (char) NULL;
                //printf("to add s fault\n");

                ll_append(&dictionary, to_add, 1 + longest_match);
                
                if(longest_entry < 1+longest_match)
                {
                    longest_entry = 1+longest_match;
                }
            }

            //printf("ADD\n%s \nADD\n %d\n",to_add, 1 +longest_match);
            
            free(read);
            printf("Total Read %d \t %d\n====================\n",bytes_read,file_size);
        }

        struct link* it = dictionary.start;
        printf("=======================\n");
        int ct = 0;
        printf("%d SOF CHAR %d\n",dictionary.length,(int)sizeof(char));
        while(it != NULL)
        {
            ct++;
            printf("{%d}\t\t",ct);
            print_bytes(it->val,it->length);
            it = it->next;
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

    
}