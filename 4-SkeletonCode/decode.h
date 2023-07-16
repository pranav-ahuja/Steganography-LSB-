#ifndef DECODE_H
#define DECODE_H

#include "types.h"

#define MAGIC_STRING_SIZE 2

typedef struct _DecodeInfo
{
    /* Image info */
    char *src_image_fname;
    FILE *fptr_src_image;

    /* Output File Info */
    char *output_fname;
    FILE *fptr_output;
    long size_output_file;
} DecodeInfo;

/*Decoding function prototype*/

//function to read and validate the arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

//function to do the decoding
Status do_decoding(DecodeInfo *decInfo, char *argv[]);

//function to open files
Status open_decode_files(char *fileName, DecodeInfo *decInfo);

//function to decode the magic string
Status decode_magic_string(DecodeInfo *decInfo);

//function to convert image to data
Status image_to_data(FILE *fptr_src, FILE *fptr_output);

//function to decode lsb values to the data
char decode_lsb_to_data(char * imageBuffer);

//function to decode the file size extention
int decode_file_size_ext(DecodeInfo *decInfo);

//function to decode the lsb values to size
int decode_lsb_to_size(char * output_extn);

//function to decode the file extention
Status decode_file_extn(int extn_size, DecodeInfo * decInfo);

//function to decode the message file extention
int decode_message_file_size(DecodeInfo *decInfo);

//function to decode the message file
Status decode_message_file(int file_size, DecodeInfo *decInfo);

//function to close the files
void closing_files(DecodeInfo * decInfo);

#endif