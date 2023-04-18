/******************************************************************************
 *
 *                              fetcher.c
 * 
 *     Assignment: um 
 *     Authors: Angela Shen and Nora A-Rahim
 *     Date: April 14, 2023
 *
 *    This file contains the implementation for fetcher, a module that 
 *    reads in Universal Machine instructions and stores them for the executor
 *    module to execute. 
 *    
 *****************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "seq.h"
#include "bitpack.h"
#include "fetcher.h"

const int INSTRUCTION_SIZE = 4;

/**************************** packProgramInstructions() *********************
 *  Purpose: Takes in a character array of four words and bitpacks them into a
 *           uint32_t in big endian
 *  Parameters: unsigned char *word: a character array of 4 characters that
 *                                   represent an instruction
 *  Returns: a uint32_t representing a Universal Machine instruction
 *  Effects: Uses bitpack module to pack a word
 *  Expects: the character array must have four values in it
 ***********************************************************************/
uint32_t packProgramInstructions(unsigned char *word) 
{
        uint32_t packed_word = 0;
        for (int i = 0; i < 4; i++) {
                packed_word = Bitpack_newu(packed_word, 8, 
                                           24 - (i * 8), word[i]);
        }
        return packed_word;
}

/**************************** loadProgramInstructions() **********************
 *  Purpose: Read in UM instructions from a file and create a sequence that
 *           contains each instruction of the entire program
 *  Parameters: char *filename: name of file containing UM instructions
 *              Seq_T segment_0: segment to have instructions put into
 *              int program_size: number of how many instructions are in the
                                  file
 *  Returns: None
 *  Effects: Uses Hanson sequence modules to modify a sequence and calls helper
 *           function packProgramInstructions to convert characters in the file
 *           to a 32 bit instruction
 *  Expects: Expects that all parameters exist and the file has UM instructions
 ***********************************************************************/
void loadProgramInstructions(char *filename, Seq_T segment_0, int program_size)
{
        int one_byte; 
        unsigned char word[4];
        FILE *fp;
        
        /* Open file */
        fp = fopen(filename, "r");
        if (fp == NULL) {
                fprintf(stderr, "Error: File cannot be opened.\n");
                exit(EXIT_FAILURE);
        }
       
        /* For every instruction in the file, create a packed word version
        of the instruction and add it to segment_0 sequence */
        for (int outer = 0; outer < program_size; outer++) {
                for (int inner = 0; inner < INSTRUCTION_SIZE; inner++) {
                        if (feof(fp)) {
                                fprintf(stderr, 
                                        "Error: Issue reading file.\n");
                                exit(EXIT_FAILURE);
                        }
                        one_byte = fgetc(fp);
                        word[inner] = one_byte;
                }
                uint32_t packed_word = packProgramInstructions(word);
                Seq_addhi(segment_0, (void *) (uintptr_t) packed_word);
        }
        fclose(fp);
}

