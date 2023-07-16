#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

//Fuunction definition for decoding
Status do_decoding(DecodeInfo *decInfo, char *argv[])
{
    printf("INFO: Opening required files\n");
    //opening the files
    if(open_decode_files(decInfo->src_image_fname, decInfo) == e_success)
    {
        printf("INFO: Opened %s\n", decInfo->src_image_fname);
    }
    else{
        return e_failure;
    }

    printf("INFO: Decoding Magic String String\n");
    //decode the magic string
    if(decode_magic_string(decInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else{
        return e_failure;
    }

    printf("INFO: Decoding Output File Extention\n");
    //Decode the file extention
    if(decode_file_extn(decode_file_size_ext(decInfo), decInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else{
        return e_failure;
    }

    //check if the output file is not present
    if(argv[3] != NULL)
    {
        //open the decode files
        if(open_decode_files(decInfo->output_fname, decInfo) == e_success)
        {
            printf("INFO: Opened %s\n", decInfo->output_fname);
        }
    }
    else{
        //else open the encode files
        printf("INFO: Ouput File not mentioned. Creating %s as default\n", decInfo->output_fname);
        if(open_decode_files(decInfo->output_fname, decInfo) == e_success)
        {
            printf("INFO: Opened %s\n", decInfo->output_fname);
        }
    }
    printf("INFO: Done. Opened all required files\n"); 

    printf("INFO: Decoding %s File Size\n", decInfo->output_fname);
    
    //decode the message file
    if(decode_message_file(decode_message_file_size(decInfo), decInfo) == e_success)
    {
        printf("INFO: Done\n");
    }

    //close the files
    closing_files(decInfo);
    return e_success;
}

//Function definition to open decode files
Status open_decode_files(char *fileName, DecodeInfo *decInfo)
{
    //check if the input file is image file
    if(!strcmp(fileName, decInfo->src_image_fname))
    {
        //open the image file
        decInfo->fptr_src_image = fopen(fileName, "r");
    }

    //check if the input file is decoded text
    else if(!strcmp(fileName, decInfo->output_fname))
    {
        //open the decoded text file
        decInfo->fptr_output = fopen(fileName, "w");
    }
    else{
        return e_failure;
    }
    return e_success;
}

//Function definition to decoede the magic string
Status decode_magic_string(DecodeInfo *decInfo)
{
    //seek the pointer till 54th position
    fseek(decInfo->fptr_src_image, 54, SEEK_SET);
    //decode image to data
    if(image_to_data(decInfo->fptr_src_image, decInfo->fptr_output) == e_success)
    {
        return e_success;
    }
    else{
        return e_failure;
    }
}

// Funtion definition to decode image to data
Status image_to_data(FILE *fptr_src, FILE *fptr_output)
{
    //declare the string buffer and magic string
    char str[8];
    char magic_string[MAGIC_STRING_SIZE];

    //start the loop 
    for(int i = 0; i<MAGIC_STRING_SIZE; i++)
    {
        //read the 8 bytes from image file
        fread(str, 1, 8, fptr_src);
        // decode the lsb data and store it in magic string
        magic_string[i] = decode_lsb_to_data(str);
    }

    //compare if the magic string is #*
    if(!strcmp(magic_string, "#*"))
    {
        return e_success;
    }
    else{
        return e_failure;
    }
}

//Function definition to convert lsb to data
char decode_lsb_to_data(char * imageBuffer)
{
    //declare output data
    char outputData;

    //start the loop for a byte
    for(int i = 7; i>=0; i--)
    {
        //logic to convert lbs bytes to data
        imageBuffer[i] = ((imageBuffer[i] & 0x01) << i);
        outputData = (outputData & (~(1<<i))) | imageBuffer[i];
    }
    
    return outputData;
}

//Function definition to decode the file extention size
int decode_file_size_ext(DecodeInfo *decInfo)
{
    //declare the extention size and string
    int extn_size;
    char str[32];

    //read 32 bytes from image file and store it in str buffer
    fread(str, 1, 32, decInfo->fptr_src_image);

    //decode lsb to size and store it in extn_size
    extn_size = decode_lsb_to_size(str);
    return extn_size;
}

//Function definition to decode the file extention
Status decode_file_extn(int extn_size, DecodeInfo * decInfo)
{
    //declare the str and output file extention
    char str[8];
    char output_file_extn[extn_size];

    //start the loop till extn_size
    for(int i = 0; i<extn_size; i++)
    {
        //read 8 bytes from image and store it in buffer
        fread(str, 1, 8, decInfo->fptr_src_image);
        //decode the lsb to data and store in the output_file_extn
        output_file_extn[i] = decode_lsb_to_data(str);
    }
    // printf("%s", output_file_extn);
    return e_success;
}

// Function definition o=to decode lsb to size
int decode_lsb_to_size(char * output_extn)
{
    //declare the size
    int size;

    //start the loop for 32 bytes
    for(int i = 31; i>=0; i--)
    {
         //logic to convert lbs bytes to size
        output_extn[i] = (output_extn[i] & 0x01) << i;
        size = (size & (~(1<<i))) | output_extn[i];
    }
    return size;
}

//Function definition to decode the message file size
int decode_message_file_size(DecodeInfo *decInfo)
{
    //declare the string
    char str[32];
    //read the 32 bytes from image and store it in str
    fread(str, 1, 32, decInfo->fptr_src_image);
    //decode the lsb to size and store it in output file size
    decInfo->size_output_file = decode_lsb_to_size(str);
    return decInfo->size_output_file;
}

//Function to decode the message file
Status decode_message_file(int file_size, DecodeInfo *decInfo)
{
    //declare the strings
    char str[8];
    char output_string[file_size];

    //start the loop
    for(int i =0; i<file_size; i++)
    {
        //read 8 bytes from image and store it in buffer
        fread(str, 1, 8, decInfo->fptr_src_image);

        //decode the lsb to data and store in the output_string
        output_string[i] = decode_lsb_to_data(str);
    }
    //write the output string in the decode file
    fwrite(output_string, 1,  file_size, decInfo->fptr_output);
}

//Function definition to close the files
void closing_files(DecodeInfo * decInfo)
{
    //close the files
    fclose(decInfo->fptr_output);
    fclose(decInfo->fptr_src_image);
}

