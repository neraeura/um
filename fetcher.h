/*************************************************************
 *
 *                     fetcher.h
 * 
 *     Assignment: um 
 *     Authors: Angela Shen and Nora A-Rahim
 *     Date: April 14, 2023
 *
 *    This file contains the interface for fetcher, a module that 
 *    reads in Universal Machine instructions and stores them for the executor
 *    module to execute.
 *    
 **************************************************************/

#ifndef FETCHER_H 
#define FETCHER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "bitpack.h"
#include "seq.h"

void 
loadProgramInstructions(char *filename, Seq_T segment_0, int program_size);

uint32_t 
packProgramInstructions(unsigned char *word);

#endif