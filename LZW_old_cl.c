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

        
        printf("MALLOC 1\n");
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

            struct byte_array* temp_barray = ba_init(1);
            temp_barray->data[0] = (char) i;
            

            ll_append(&dictionary, temp_barray);
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
            printf("====================%d\t%d\n",dictionary.length,bytes_read);
            lc+=1;
            char* read = NULL;
            unsigned int read_length = 0;
            if(file_size - bytes_read >= longest_entry + 1)
            {
                read_length = longest_entry + 1;
                bytes_read += read_length;
                printf("MALLOC 3\n");
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
                for(int i = 0; i < read_length && i < iter->val->length;i++)
                {
                    if(iter->val->data[i] != read[i])
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
                        longest_match_buf = iter->val->data;
                        match_index = ik;
                    }

                }
                //printf("LL ITER %d %p\n", lc, iter->next);
                iter = iter->next;
                ik++;
            }
            //printf("%s longest match ptr\n",longest_match_buf);
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
            //printf("%d MATHC IND\n", match_index);
            //fwrite(&match_index, 4, 1, wp);
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
            if(!skip_dict)
            {

                struct byte_array* to_add = ba_init(longest_match + 1);
                
                printf("MALLOC 5\n");
                memcpy(to_add->data, read, 1 + longest_match);

                //Seek back unused input
                //fseek(fp, -1*(read_length - longest_match), SEEK_CUR);

                //to_add[2 + longest_match] = (char) NULL;
                //printf("to add s fault\n");

                ll_append(&dictionary, to_add);
                
                if(longest_entry < 1+longest_match)
                {
                    longest_entry = 1+longest_match;
                }
            }

            //printf("ADD\n%s \nADD\n %d\n",to_add, 1 +longest_match);
            
            //free(read);
            //printf("Total Read %d \t %d\n====================\n",bytes_read,file_size);
        }

        struct link* it = dictionary.start;
        //printf("=======================\n");
        int ct = 0;
        printf("%d SOF CHAR %d\n",dictionary.length,(int)sizeof(char));
        while(it != NULL)
        {
            ct++;
            printf("{%d}\t\t",ct);
            print_bytes(it->val->data,it->val->length);
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

        struct linked_list dictionary;
        ll_init(&dictionary);

        
        for(unsigned int i = 0; i < 256;i++)
        {

            struct byte_array* temp_barray = ba_init(1);
            temp_barray->data[0] = (char) i;
            

            ll_append(&dictionary, temp_barray);
        }

        

        unsigned int longest_entry = 1;
        int co = 0;
        unsigned int bytes_read = 0;
        printf("=======================\n");
        while(bytes_read < file_size)
        {
            co++;
            printf("=========================\n");

            char leading_byte = 0;

            unsigned int index = 0;
            fread(&leading_byte, 1, 1, fp);
            printf("%x LEADING BYTE\n",0xFF & (unsigned int)leading_byte);
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

            

            printf("%d %d\n", index, co);

            struct link* iter = dictionary.start;

            
            for(unsigned int i = 0; i < index;i++)
            {
                iter = iter->next;
            }
            //printf("%p \n", iter);
            //printf("%s %d\n", iter->val,index);
            fwrite(iter->val->data, 1, iter->val->length,wp);
            index = 0;
            leading_byte = 0;
            fread(&leading_byte, 1, 1, fp);
            printf("%x LEADING BYTE\n",0xFF & (unsigned int)leading_byte);

            unsigned int bytes_used = 0;
            if((leading_byte & 0x80) == 0)
            {
                index = 0xFF & (unsigned int)leading_byte;
            }
            else
            {
                unsigned int bytes_needed = 0xFF & (unsigned int) (leading_byte ^ 0x80);
                bytes_used=bytes_needed;
                printf("%d bytes needeed multi\n",bytes_needed);
                fread(&index, bytes_needed, 1, fp);
            }
            
            for(int i = bytes_used - 1; i >= 0;i--)
            {
                //printf("%x HEX UNGET\n", (char)0xFF && (index >> (8 * i)),fp);
                ungetc((char)0xFF & (index >> (8 * i)),fp);
            }
            ungetc(leading_byte, fp);

            printf("MALLOC 8\n");


            struct byte_array* add_data = ba_init(iter->val->length + 1);

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
            //printf("=========================\n");
        }
        fclose(fp);
        fclose(wp);
    }

    
}
