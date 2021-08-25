
#include "um_create_mem.h"
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

//return length of file to determine size of array we need
int program_length(FILE* fp){
    fseek(fp, 0, SEEK_END);
    //length in byte
    int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    //get length in 4 bytes
    return length/4;
}

//given the input file, convert into a valid zero segment to return
void creat_mem(Seq_T mem, FILE *fname){

    int length = program_length(fname);
    Array_T zero_seg = Array_new(length, sizeof(uint32_t));
    for (int i = 0; i < length; i++){
        unsigned char cha;
        //instead of a forloop here we are using hardcoded values because it would be faster with out the math
        fread(&cha, sizeof(cha), 1, fname);
        uint32_t packedByte = 0;

        //bitshifting to convert symbol into 4 char values
        packedByte = ((uint32_t)cha << 24) | packedByte;        
        fread(&cha, sizeof(cha), 1, fname);
        packedByte = ((uint32_t)cha << 16) | packedByte;
        fread(&cha, sizeof(cha), 1, fname);
        packedByte = ((uint32_t)cha << 8) | packedByte;
        fread(&cha, sizeof(cha), 1, fname);
        packedByte = ((uint32_t)cha) | packedByte;
        uint32_t *packedByte_pointer = (uint32_t *)malloc(sizeof(uint32_t));
        packedByte_pointer = &packedByte;
        Array_put(zero_seg, i, (void*)packedByte_pointer);
    }

    Array_T *store = (Array_T *)malloc(sizeof(Array_T));
    *store = zero_seg;
    Seq_addhi(mem, (void *)store);

}