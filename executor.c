/*************************************************************
 *
 *                     executor.c
 * 
 *     Assignment: um 
 *     Authors: Angela Shen and Nora A-Rahim
 *     Date: April 14, 2023
 *
 *    This file contains the implementation for executor, a module that
 *    executes the universal machine instructions of an entire program. 
 *    
 **************************************************************/

#include <stdio.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include "assert.h"
#include "bitpack.h"
#include "executor.h"
#include "instructionSet.h"
#include "memory.h"
#include "registers.h"
#include "seq.h"

typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        COND_MOV = 0, SEG_LOAD, SEG_STORE, ADD, MULT, DIV,
        NAND, HALT, MAP, UNMAP, OUTPUT, INPUT, LOAD_PROGRAM, LOAD_VAL
} Um_opcode;

/* Struct that stores the unpacked values of a load value instruction */
struct loadInstruction 
{
        int ra; 
        uint32_t value;
};

/* Struct that stores the unpacked values of any instruction that is not
load value */
struct generalInstruction 
{
        uint32_t opcode; 
        int ra, rb, rc;
};

/******************************** execute() *******************************
 *  Purpose: Executes the instructions of the entire program.
 *  Parameters: Seq_T segment_0: the 0th segment representing all the
                                 instructions of a program, created with
                                by the functions in fetcher.c
 *  Returns: None
 *  Effects: Takes in a segment created by fetcher.c and calls functions from
 *           bitpack, instructionSet, memory, Hanson sequence, and register
 *           modules
 *  Expects: segment_0 must exist, keeps running until halt instruction or end
 *           of file is reached
 ***********************************************************************/
void execute(Seq_T segment_0)
{
        /* necessary data items initialized */
        uint32_t pc = 0;
        Seq_T registers = createRegisters();
        Seq_T mapped_segments = Seq_new(0);
        Seq_T unmapped_identifiers = Seq_new(0);
        addSegToMemory(mapped_segments, segment_0);
        bool executing = true;

        /* Continues to execute until halt instruction is reached */
        while (executing) {
                uint32_t instruction = getWord(
                                       getSegment(mapped_segments, 0), pc);
                int opcode = Bitpack_getu(instruction, 4, 28);
                
                loadInstruction loadInfo;
                generalInstruction genInfo;
                if (opcode == 13) {
                        loadInfo = unpackLoad(instruction);
                } else {
                        genInfo = unpackGeneral(instruction);
                }
                
                switch (opcode) {
                        
                        case COND_MOV:
                        conditionalMove(registers, genInfo->ra,
                                                   genInfo->rb, 
                                                   genInfo->rc);
                        break;

                        case SEG_LOAD:
                        segLoad(registers, mapped_segments, genInfo->ra,
                                                            genInfo->rb,
                                                            genInfo->rc);
                        break;
                        
                        case SEG_STORE:
                        segStore(registers, mapped_segments, genInfo->ra,
                                                             genInfo->rb,
                                                             genInfo->rc);
                        break;
                      
                        case ADD:
                        add(registers, genInfo->ra, genInfo->rb, genInfo->rc);
                        break;

                        case MULT:
                        multiply(registers, genInfo->ra,
                                            genInfo->rb,
                                            genInfo->rc);
                        break;

                        case DIV:
                        divide(registers, genInfo->ra,
                                          genInfo->rb, 
                                          genInfo->rc);
                        break;

                        case NAND: 
                        nand(registers, genInfo->ra, genInfo->rb, genInfo->rc);
                        break;

                        case HALT:
                        executing = false;
                        break;
                        
                        case MAP:
                        mapSegment(registers, mapped_segments,
                                              unmapped_identifiers,
                                              genInfo->rb, genInfo->rc);
                        break;

                        case UNMAP:
                        unmapSegment(registers, mapped_segments,
                                                unmapped_identifiers,
                                                genInfo->rc);
                        break;

                        case OUTPUT:
                        output(registers, genInfo->rc);
                        break;

                        case INPUT:
                        input(registers, genInfo->rc);
                        break;

                        case LOAD_PROGRAM:
                        loadProgram(mapped_segments, unmapped_identifiers,
                                                     registers, genInfo->rb,
                                                     genInfo->rc, &pc);
                        break;
                        
                        case LOAD_VAL:
                        loadValue(registers, loadInfo->ra, loadInfo->value);
                        break;

                        default:
                        fprintf(stderr, "Not a valid instruction\n");
                        exit(EXIT_FAILURE);
                }   
                
                /* If loadprogram is called then we do NOT want to
                increment the program counter */
                if (opcode != 12) {
                        pc++;
                }    
                
                /* free newLoad and newGeneral structs */
                if (opcode == 13) {
                        free(loadInfo);
                } else {
                        free(genInfo);
                }
        }

        /* free registers */
        Seq_free(&registers);

        /* free unmapped_identifiers */
        Seq_free(&unmapped_identifiers);

        /* free mapped_segments */
        for(int i = 0; i < segmentLength(mapped_segments); i++)
        {
                assert(mapped_segments != NULL);
                Seq_T segment = Seq_get(mapped_segments, i);
                if (segment != NULL) { 
                        Seq_free(&segment);
                }
        }
        Seq_free(&mapped_segments);
}

/**************************** newLoad() ****************************
 *  Purpose: Initializes a loadInstruction struct
 *  Parameters: None
 *  Returns: a new loadInstruction struct with members set to 0
 *  Effects: Mallocs space for a new struct
 *  Expects: None
 ***********************************************************************/
loadInstruction newLoad() {
        loadInstruction new = malloc(sizeof(struct loadInstruction));
        assert(new);
        new->ra = 0;
        new->value = 0;

        return new;
}

/**************************** newGeneral() ****************************
 *  Purpose: Initializes a generalInstruction struct
 *  Parameters: None
 *  Returns: a new generalInstruction struct with members set to 0
 *  Effects: Mallocs space for a new struct
 *  Expects: None
 ***********************************************************************/
generalInstruction newGeneral() {
        generalInstruction new = malloc(sizeof(struct generalInstruction));
        assert(new);
        new->opcode = 0;
        new->ra = 0;
        new->rb = 0;
        new->rc = 0;

        return new;
}

/**************************** unpackLoad() ****************************
 *  Purpose: Unpacks a 32 bit instruction for and returns a struct containing 
 *           the unbitpacked information, for load value instruction format
 *           specifically
 *  Parameters: uint32_t instruction: universal machine load value instruction
 *  Returns: a loadInstruction struct with values corresponding to the 
             bitpacked instruction
 *  Effects: Mallocs space for a new struct and uses bitpack module to unpack
 *  Expects: instruction must be valid and packed in the correct manner
 ***********************************************************************/
loadInstruction unpackLoad(uint32_t instruction) 
{
        /* instantantiate a loadInstruction struct */
        loadInstruction newInstruction = newLoad();

        /* using Bitpack_getu, extract r[A], and value */
        newInstruction->ra = Bitpack_getu(instruction, 3, 25);
        newInstruction->value = Bitpack_getu(instruction, 25, 0);
        
        return newInstruction;
}

/**************************** unpackGeneral() ****************************
 *  Purpose: Unpacks a 32 bit instruction and returns a struct containing the
 *           unbitpacked information
 *  Parameters: uint32_t instruction: a universal machine instruction
 *  Returns: a generalInstruction struct with values corresponding to the 
             bitpacked instruction
 *  Effects: Mallocs space for a new struct and uses bitpack module to unpack
 *  Expects: instruction must be valid and packed in the correct manner
 ***********************************************************************/
generalInstruction unpackGeneral(uint32_t instruction)
{
        generalInstruction newInstruction = newGeneral();
        newInstruction->opcode = Bitpack_getu(instruction, 4, 28);
        newInstruction->ra = Bitpack_getu(instruction, 3, 6);
        newInstruction->rb = Bitpack_getu(instruction, 3, 3);
        newInstruction->rc = Bitpack_getu(instruction, 3, 0);

        return newInstruction;
}


