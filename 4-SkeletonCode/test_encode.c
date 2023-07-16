#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"

int main(int argc, char * argv[])
{
    //declare the structure variable for decode ad encode
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    //check if the operation is encode
    if(check_operation_type(argv) == e_encode)
    {
        //check if reading and validating the variables is correct or not
        if(read_and_validate_encode_args(argv, &encInfo) == e_success)
        {          
            //checking if the encoding is done or not
            if(do_encoding(&encInfo) == e_success)
            {
                printf("INFO: ## Encoding Done Successfully ##\n");
            }
            else{
                printf("Encoding failed.\n");
            }
        }
        else{
            printf("Read and Validate agruments failed.\n");
        }
    }
    //check if the operation is decode
    else if(check_operation_type(argv) == e_decode)
    {
        printf("## Decoding Procedure Started ##\n");

        //check if reading and validating the variables is correct or not
        if(read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            //checking if the decoding is done or not
            if(do_decoding(&decInfo, argv) == e_success)
            {
                printf("## Decoding Done Successfully ##\n");
            }
            else{
                printf("Failure\n");
            }
        }
    }
    else{
        printf("Please enter correct operation.\n");
    }

    return 0;
}

//function to check the operation types
OperationType check_operation_type(char *argv[])
{
    //check if the argument is -e encode
    if(!strcmp(argv[1], "-e"))
    {
        return e_encode;
    }
    //check if the argument is -d decode
    else if(!strcmp(argv[1], "-d"))
    {
        return e_decode;
    }
    //else return e_unsupported
    else{
        return e_unsupported;
    }
}

//function to read and validating the encoding arguments
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //check if the second argument is NULL 
    if(argv[2] != NULL)
    {
        //check if the second argument is .bmp file
        if(!strcmp(strchr(argv[2], '.'), ".bmp"))
        {
            //if so then assign the source image file name
            encInfo->src_image_fname =  argv[2];
        }
    }
    else{
        //else enter the correct .bmp file
        printf("ERROR: Enter the correct bmp file.\n");
        return e_failure;
    }

    //check if the thrid argument is not NULL
    if(argv[3] != NULL)
    {
        //if true then assign the argument 3 to secret file name
        encInfo->secret_fname = argv[3];
    }
    else{
        //if not then give error that secret file is not there
        printf("ERROR: No secret file entered\n");
        return e_failure;
    }

    //check if the fourth argument is not NULL
    if(argv[4] != NULL)
    {
        // if so then assign the fourth argument to stego image name 
        encInfo->stego_image_fname = argv[4];
    }
    else{
        //else assign a default name
        encInfo->stego_image_fname = "steged_img.bmp";
        printf("INFO: Output file not mentioned. Creating steged_img.bmp as default.\n");
    }

    return e_success;
}

//function to read and validating the decoding arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    //check if the second argument is NULL 
    if(argv[2] != NULL)
    {
        //check if the second argument is .bmp file
        if(!strcmp(strchr(argv[2], '.'), ".bmp"))
        {
            //if so then assign the source image file name
            decInfo->src_image_fname = argv[2];
        }
        else{
            return e_failure;
        }

        //check if the thrid argument is not NULL
        if(argv[3] != NULL)
        {
            // if so then assign the third argument to stego image name
            decInfo->output_fname = argv[3];
        }
        else{
            //else assign a default name
            decInfo->output_fname = "decoded.txt";
        }
    }
    return e_success;
}