#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files.\n");
    //Opening the files
    if(open_files(encInfo) == e_success)
    {   
        printf("INFO: Done.\n");
    }
    else{
        printf("ERROR: Cannot open file/files\n");
        return e_failure;
    }

    printf("INFO: ## Encoding Procedure Started ##\n");
    
    //checking capacity
    if(check_capacity(encInfo) == e_success)
    {
        printf("INFO: Done. Found OK\n");
    }
    else{
        printf("ERROR: Not OK\n");
        return e_failure;
    }

    //Copying the bmp header
    printf("INFO: Copying Image Header\n");
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: Done\n");
    }
    else{
        printf("ERROR: Error\n");
        return e_failure;
    }    
    
    //Encoding magic string to the stego file
    printf("INFO: Encoding Magic String Signature\n");
    if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else{
        printf("ERROR: Error\n");
        return e_failure;
    }

    //Encoding secret file extention to the stego file
    printf("INFO: Encoding %s File Extention\n", encInfo->secret_fname);
    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
    {
        printf("INFO: Done.\n");
    }
    else{
        printf("ERROR: Error\n");
        return e_failure;
    }

    //Encoding secret file size to stego image
    printf("INFO: Encoding %s File Size\n", encInfo->secret_fname);
    if(encode_secret_file_size(get_file_size(encInfo), encInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else{
        printf("ERROR: Error.\n");
        return e_failure;
    }

    //Encoding the secret file data to stego image
    printf("INFO: Encoding %s File Data\n", encInfo->secret_fname);
    if(encode_secret_file_data(encInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else{
        printf("ERROR: Error\n");
        return e_failure;
    }

    //copying the remaining data to stego image
    printf("INFO: Copying Left Over Data\n");
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: Done\n");
    }
    else{
        printf("ERROR: Error\n");
        return e_failure;
    }

    //closing each file
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_stego_image);
    return e_success;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("%s\n", encInfo->src_image_fname);
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
       
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    printf("INFO: Opening %s.\n", encInfo->src_image_fname);

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    printf("INFO: Opening %s.\n", encInfo->secret_fname);
    //copyting the extention of secret file to a variable
    strcpy(encInfo->extn_secret_file, strchr(encInfo->secret_fname, '.'));

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    printf("INFO: Opening %s.\n", encInfo->stego_image_fname);
    // No failure return e_success
    return e_success;
}

//FUNCTION TO CHECK THE CAPACITY
Status check_capacity(EncodeInfo *encInfo)
{
    printf("INFO: Checking for %s size\n", encInfo->secret_fname);
    int secretSize = get_file_size(encInfo);
    if(secretSize != 0)
    {
        printf("INFO: Done. Not Empty\n");
    }
    else{
        printf("ERROR: The file is empty\n");
        return e_failure;
    }

    int src_img_size = get_file_size(encInfo);
    printf("INFO: Checking for %s capacity to handle %s\n", encInfo->src_image_fname, encInfo->secret_fname);

    if(secretSize <= src_img_size)
    {
        return e_success;
    }
    else{
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    //declare a character array of 54 characters. This is the bmp header
    char str[54];
    //moving the cursor to the startiung of the source image file using rewind function
    rewind(fptr_src_image);
    //reading characters from 1 to 54 from source image file and storing it in str array buffer
    fread(str, 1, 54, fptr_src_image);
    //moving the cursor to the startiung of the stego image file using rewind function
    rewind(fptr_dest_image);
    //writing characters from 1 to 54 to stego image file from the array buffer
    fwrite(str, 1, 54, fptr_dest_image);


    return e_success;
}

//FUNCTION TO ENCODE THE MAGIC STRING
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    //calling a function to move the data to image
    encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

//FUNCTION TO SEND DATA TO IMAGE
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    // declare the character srray of 8 characters
    char str[8];

    //start the loop to convert ecah character of string to lsb bits
    for(int i = 0; i<size; i++)
    {
        //read the first 8 bytes from the source image and store it in the character array buffer
        fread(str, 1, 8, fptr_src_image);
        //Call the function to convert each byte to the LSB
        encode_byte_to_lsb(data[i], str);
        //write the first 8 bytes to the stego image and store it in the character array buffer
        fwrite(str, 1, 8, fptr_stego_image);
    }
    return e_success;
}

//FUNCTION TO CONVERT EACH BYTE TO LSB
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    //start the loop for traversing through each bit of a byte. This is only for character data(1 byte)
    for(int i = 7; i>=0; i--)
    {
        image_buffer[i] = ((data & (1<<i)) >> i) | (image_buffer[i] & 0xFE);
    }
}

// FUNCTION TO ENCODE THE SECRET FILE EXTENTION SIZE TO STEGO FILE
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{   //calling a function to move the size to image
    encode_size_to_image(size, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

//FUNCTION TO SEND SIZE TO IMAGE
Status encode_size_to_image(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    // declare the character srray of 32 characters
    char str[32];

    //read the first 32 bytes from the source image and store it in the character array buffer
    fread(str, 1, 32, fptr_src_image);
    //Call the function to convert each byte of integer size to the LSB
    encode_size_to_lsb(size, str);
    //read the first 32 bytes from the source image and store it in the character array buffer
    fwrite(str, 1, 32, fptr_stego_image);
}

//FUNCTION TO CONVERT EACH BYTE OF SIZE TO LSB
Status encode_size_to_lsb(int data, char *image_buffer)
{
    //start the loop for traversing through each bit of a byte. This is only for integer data(4 byte)
    for(int i = 32; i>=0; i--)
    {
        image_buffer[i] = ((data & (1<<i)) >> i) | (image_buffer[i] & 0xFE);
    }
}

// FUNCTION TO ENCODE THE SECRET FILE EXTENTION TO STEGO FILE
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
    {
     
        //calling a function to move the data to image
        if(encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
        {
            return e_success;
        }
    }   
    else{
        return e_failure;
    }
}

// FUNCTION TO GET FILE SIZE
int get_file_size(EncodeInfo * encInfo)
{
    //seek the curser to the file end
    fseek(encInfo->fptr_secret, 0L, SEEK_END);

    //get the position of cursor from the file
    encInfo->size_secret_file = ftell(encInfo->fptr_secret);

    //return the file size
    return encInfo->size_secret_file;
}

// FUNCTION TO ENCODE THE SECRET FILE SIZE
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    //call function to encode size to image
    encode_size_to_image(file_size, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

//ENCODE SECRET FILE DATA
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //declare the charcater variable
    char str[encInfo->size_secret_file];

    rewind(encInfo->fptr_secret);
    fread(str, 1, encInfo->size_secret_file, encInfo->fptr_secret);
    encode_data_to_image(str, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image);

    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    // declare the character variable
    char ch;
    //read each character from source file
    while(fread(&ch, 1, 1, fptr_src)>0)
    {
        //write each character to the destination file
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return e_success;
}
