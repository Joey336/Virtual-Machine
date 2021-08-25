#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "seq-Lw.h"
#include "array-Lw.h"
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
    uint32_t *program = creat_mem(mem, fp);
    
    uint32_t *testReg;
    testReg = malloc(8 * sizeof(uint32_t));
    //create registers
    
   

    //for use in map/unmap
    Seq_T freeIndices = Seq_new(0);
    uint32_t hwm = 0;

    
    //program represents current 0 segment
    //Array_T program = *(Array_T *)Seq_get(mem, 0);

 
    int program_counter = 0;
    
    uint32_t *reg_A_pointer;
    uint32_t *reg_B_pointer;
    uint32_t *reg_C_pointer;

    uint32_t reg_A;
    uint32_t reg_B;
    uint32_t reg_C;
    //infinite while loop runs until halt (opcode7) is reached
    while (1)
    {

        //Main memory data sruct
        uint32_t segment = program[program_counter];

        //optcode
        uint32_t optcode = segment >> 28;


        //printf("%d\n",optcode);
        //main switch statement to hop to specific opcodes
        switch (optcode){
        
        //Cond. Move (copies regB to regA if regC != 0)
        case 0: ;
            reg_C = segment << 29 >> 29;
            reg_C_pointer = &testReg[reg_C];
            if(*reg_C_pointer == 0){
                break;
            }

            reg_A = segment << 23 >> 29;
            reg_B = segment << 26 >> 29;
            reg_A_pointer = &testReg[reg_A];
            reg_B_pointer = &testReg[reg_B];
  
            if((*reg_C_pointer) != 0){

                *reg_A_pointer = *reg_B_pointer;
                //assert(*reg_A_pointer == *reg_B_pointer);
            }
            break;

        //Seg Load (loads word from m[regB][regC] into regA)
        case 1: ;
            reg_A = segment << 23 >> 29;
            reg_B = segment << 26 >> 29;
            reg_C = segment << 29 >> 29;
            reg_A_pointer = &testReg[reg_A];
            reg_B_pointer = &testReg[reg_B];
            reg_C_pointer = &testReg[reg_C];
            //assert((unsigned)Seq_length(mem) > (unsigned)*reg_B_pointer);
            *reg_A_pointer = *(uint32_t *)Array_get(*(Array_T *)Seq_get(mem, *reg_B_pointer), *reg_C_pointer);
            break;

        //Seg Store (loads regC into m[regA][regB])
        case 2: ;
            reg_A = segment << 23 >> 29;
            reg_B = segment << 26 >> 29;
            reg_C = segment << 29 >> 29;
            reg_A_pointer = &testReg[reg_A];
            reg_B_pointer = &testReg[reg_B];
            reg_C_pointer = &testReg[reg_C];
            uint32_t *word = (uint32_t *)Array_get(*(Array_T *)Seq_get(mem, *reg_A_pointer), *reg_B_pointer);
            *word =  *reg_C_pointer;
            //assert(*(uint32_t *)Array_get(*(Array_T *)Seq_get(mem, *reg_A_pointer), *reg_B_pointer) == *reg_C_pointer);
            break;

        //Addition (RegA = RegB + RegC)
        case 3: ;
            reg_A = segment << 23 >> 29;
            reg_B = segment << 26 >> 29;
            reg_C = segment << 29 >> 29;
            reg_A_pointer = &testReg[reg_A];
            reg_B_pointer = &testReg[reg_B];
            reg_C_pointer = &testReg[reg_C];
            *reg_A_pointer = (uint32_t)*reg_B_pointer + (uint32_t)*reg_C_pointer;
            break;

        //Multiplication (RegA = RegB * RegC)
        case 4: ;
            reg_A = segment << 23 >> 29;
            reg_B = segment << 26 >> 29;
            reg_C = segment << 29 >> 29;
            reg_A_pointer = &testReg[reg_A];
            reg_B_pointer = &testReg[reg_B];
            reg_C_pointer = &testReg[reg_C];
            *reg_A_pointer = (uint32_t)(*reg_B_pointer) * (uint32_t)(*reg_C_pointer);
            break;

        //Division (RegA = RegB / RegC)
        case 5: ;
            reg_A = segment << 23 >> 29;
            reg_B = segment << 26 >> 29;
            reg_C = segment << 29 >> 29;
            reg_A_pointer = &testReg[reg_A];
            reg_B_pointer = &testReg[reg_B];
            reg_C_pointer = &testReg[reg_C];
            *reg_A_pointer = (uint32_t)((uint32_t)(*reg_B_pointer) / (uint32_t)(*reg_C_pointer));
            break;

        //NAND (RegA = ~(RegB & RegC)
        case 6: ;
            reg_A = segment << 23 >> 29;
            reg_B = segment << 26 >> 29;
            reg_C = segment << 29 >> 29;
            reg_A_pointer = &testReg[reg_A];
            reg_B_pointer = &testReg[reg_B];
            reg_C_pointer = &testReg[reg_C];
            *reg_A_pointer = ~(*reg_B_pointer & *reg_C_pointer);
            break;

        //Halt, Simply exit(0)
        case 7: 
     
            exit(0);
            break;

        //Map Segment (create new array of size RegC to store in main memory segment)
        case 8: ;
            reg_B = segment << 26 >> 29;
            reg_C = segment << 29 >> 29;
            reg_B_pointer = &testReg[reg_B];
            reg_C_pointer = &testReg[reg_C];
            Array_T new_seg = Array_new(*reg_C_pointer, sizeof(uint32_t*));
            
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
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
            reg_C = segment << 29 >> 29;
            reg_C_pointer = &testReg[reg_C];
            Array_T *free_arr = (Array_T *)Seq_get(mem, *reg_C_pointer);
            Array_free(free_arr);
            //here just so we are not passing the register pointer around
            uint32_t *index = (uint32_t *)malloc(sizeof(uint32_t));
            *index = *reg_C_pointer;
            Seq_addhi(freeIndices, index); 
            break;

        //Output (print ascii representation of RegC)
        case 10: ;
            reg_C = segment << 29 >> 29;
            reg_C_pointer = &testReg[reg_C];
            printf("%c", (char)*reg_C_pointer);
            break;

        //Input (takes input from stdin to store in RegC)
        case 11: ;
            reg_C = segment << 29 >> 29;
            reg_C_pointer = &testReg[reg_C];
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
        case 12: ;
            reg_B = segment << 26 >> 29;
            reg_C = segment << 29 >> 29;
            reg_B_pointer = &testReg[reg_B];
            reg_C_pointer = &testReg[reg_C];
            if(*reg_B_pointer != 0){  
                Array_T new_prog = *(Array_T *)Seq_get(mem, *reg_B_pointer);
                //assert(new_prog);
                free(program);
                program = malloc(Array_length(new_prog) * sizeof(uint32_t));
                for(int i = 0 ; i <  Array_length(new_prog); i++){
                    program[i] = *(uint32_t *)Array_get(new_prog, i);
                }
                Array_T copy_new_prog = Array_copy(new_prog, Array_length(new_prog));
                Seq_remlo(mem);
                
                Array_T *re_pro = (Array_T *)malloc(sizeof(Array_T));
                *re_pro = copy_new_prog;
                //program = copy_new_prog;
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
            //printf("REG: %u VALUE: %u\n",testReg[regId], value);
            testReg[regId] = value;
            //printf("%u ",testReg[regId]);
          
            //uint32_t *this = &testReg[regId];
            //*this = value;
            //assert(*(uint32_t*)Array_get(reg, regId) == value);
            break;

        default:
            break;
        }
        
        program_counter += 1;

    }
    
    

}


