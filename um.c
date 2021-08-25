#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "seq.h"
#include "array.h"
#include "um_create_mem.h"


int main(int argc, char *argv[]){
    (void) argc;

    assert(argc == 2);
    FILE *fp;
    //open file
    fp = fopen(argv[1], "rb");
    assert(fp);
    

    //get the memory with program in it
    Seq_T mem = Seq_new(1);
    creat_mem(mem, fp);

    //create registers
    Array_T reg = Array_new(8, sizeof(uint32_t));

    for (int i = 0; i < 8; i++){
        uint32_t *reg_ = (uint32_t*)malloc(sizeof(uint32_t));
        *reg_ = 0;
        Array_put(reg, i, (void *)reg_);
    }

    //for use in map/unmap
    Seq_T freeIndices = Seq_new(0);
    uint32_t hwm = 0;

    //program represents current 0 segment
    Array_T program = *(Array_T *)Seq_get(mem, 0);
    int program_counter = 0;

  
    //infinite while loop runs until halt (opcode7) is reached
    while (1)
    {

        //Main memory data sruct
        uint32_t segment = *(uint32_t *)Array_get(program, program_counter);

        //optcode
        uint32_t optcode = segment >> 28;

        //register A
        uint32_t reg_A = segment << 23;
        reg_A = (unsigned)reg_A >> 29;
        uint32_t *reg_A_pointer = (uint32_t *)Array_get(reg, (int)reg_A);
        
        //register B
        uint32_t reg_B = segment << 26;
        reg_B = (unsigned)reg_B >> 29;
        uint32_t *reg_B_pointer = (uint32_t *)Array_get(reg, (int)reg_B);

        //register C
        uint32_t reg_C = segment << 29;
        reg_C = (unsigned)reg_C >> 29;
        uint32_t *reg_C_pointer = (uint32_t *)Array_get(reg, (int)reg_C);

        //main switch statement to hop to specific opcodes
        switch (optcode){

        //Cond. Move (copies regB to regA if regC != 0)
        case 0:
            if((*reg_C_pointer) != 0){
                uint32_t v = *reg_B_pointer;
                *reg_A_pointer = v;
                assert(*reg_A_pointer == *reg_B_pointer);
            }
            break;

        //Seg Load (loads word from m[regB][regC] into regA)
        case 1:
            assert((unsigned)Seq_length(mem) > (unsigned)*reg_B_pointer);
            uint32_t value2 = *(uint32_t *)Array_get(*(Array_T *)Seq_get(mem, *reg_B_pointer), *reg_C_pointer);
            *reg_A_pointer = value2;
            break;

        //Seg Store (loads regC into m[regA][regB])
        case 2: ;
            uint32_t value_store = *reg_C_pointer;
            Array_T a = *(Array_T *)Seq_get(mem, *reg_A_pointer);
            uint32_t *word = (uint32_t *)Array_get(a, *reg_B_pointer);
            *word = value_store;
            assert(*(uint32_t *)Array_get(*(Array_T *)Seq_get(mem, *reg_A_pointer), *reg_B_pointer) == *reg_C_pointer);
            break;

        //Addition (RegA = RegB + RegC)
        case 3:
            *reg_A_pointer = (uint32_t)*reg_B_pointer + (uint32_t)*reg_C_pointer;
            break;

        //Multiplication (RegA = RegB * RegC)
        case 4:
            *reg_A_pointer = (uint32_t)(*reg_B_pointer) * (uint32_t)(*reg_C_pointer);
            break;

        //Division (RegA = RegB / RegC)
        case 5:
            *reg_A_pointer = (uint32_t)((uint32_t)(*reg_B_pointer) / (uint32_t)(*reg_C_pointer));
            break;

        //NAND (RegA = ~(RegB & RegC)
        case 6:
            *reg_A_pointer = ~(*reg_B_pointer & *reg_C_pointer);
            break;

        //Halt, Simply exit(0)
        case 7: 
            exit(0);
            break;

        //Map Segment (create new array of size RegC to store in main memory segment)
        case 8: ;
            Array_T new_seg = Array_new(*reg_C_pointer, sizeof(uint32_t*));
            
            for(int j = 0; j < Array_length(new_seg); j++){
                uint32_t *packedByte_pointer = (uint32_t *)malloc(sizeof(uint32_t));
                *packedByte_pointer = 0;
                uint32_t *arr = (uint32_t *)Array_get(new_seg, j);
                *arr = *packedByte_pointer;
            }
            Array_T *new_maped_seg_pointer = (Array_T *)malloc(sizeof(Array_T));
            *new_maped_seg_pointer = new_seg;

            //if there is an index that is availible (index was unmapped previously)
            if(Seq_length(freeIndices) != 0){
                //not sure if remhi also frees the index but we are going to find out in the next assigment
                uint32_t index = *(uint32_t *)Seq_remhi(freeIndices);
                Seq_put(mem, index ,new_maped_seg_pointer);
                *reg_B_pointer = index;

            }
            else{
                hwm+=1;
                Seq_addhi(mem, new_maped_seg_pointer);
                *reg_B_pointer = hwm;
            }
            break;

        //Unmap Segment (frees segment at m[RegC] and stores its index for reuse)
        case 9: ; 
            Array_T *free_arr = (Array_T *)Seq_get(mem, *reg_C_pointer);
            Array_free(free_arr);
            //here just so we are not passing the register pointer around
            uint32_t *index = (uint32_t *)malloc(sizeof(uint32_t));
            *index = *reg_C_pointer;
            Seq_addhi(freeIndices, index); 
            break;

        //Output (print ascii representation of RegC)
        case 10:
            printf("%c", (char)*reg_C_pointer);
            break;

        //Input (takes input from stdin to store in RegC)
        case 11: ;
            char input = getc(stdin);
            //if EOF fill regC with 2^32 - 1 instead
            if(input == EOF){
                *reg_C_pointer = ~0;
            }
            else{
                *reg_C_pointer = (uint32_t) input;
            }
            break;

        //Load Program (Dupes segment at m[RegB] and replaces m[0] with it)
        //also sets program counter to RegC
        case 12:
            if(*reg_B_pointer != 0){  
                Array_T new_prog = *(Array_T *)Seq_get(mem, *reg_B_pointer);
                assert(new_prog);
                Array_T copy_new_prog = Array_copy(new_prog, Array_length(new_prog));
                Seq_remlo(mem);
                
                Array_T *re_pro = (Array_T *)malloc(sizeof(Array_T));
                *re_pro = copy_new_prog;
                program = copy_new_prog;
                Seq_addlo(mem, (void *)re_pro);
               
            }
            program_counter = *reg_C_pointer - 1;
            break;

        //Load Value (stores first 25 bits of word into reg index that bits 25-27 of word represents)
        case 13: ;
            //bitshifting to islate reg index we are storing to
            uint32_t regId = segment << 4;
            regId = (unsigned)regId >> 29;

            //bitshifting to isolate value we want to store
            uint32_t value = segment << 7;
            value = (unsigned)value >> 7;
            
            uint32_t *this = (uint32_t *)Array_get(reg, regId);
            *this = value;
            assert(*(uint32_t*)Array_get(reg, regId) == value);
            break;

        default:
            break;
        }
        program_counter += 1;

    }
    
    

}
