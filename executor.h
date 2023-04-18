/*************************************************************
 *
 *                     executor.h
 * 
 *     Assignment: um 
 *     Authors: Angela Shen and Nora A-Rahim
 *     Date: April 14, 2023
 *
 *    This file contains the interface for executor, a module that
 *    executes the universal machine instructions of an entire program. 
 *    
 **************************************************************/
#ifndef EXECUTOR_H
#define EXECUTOR_H

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

typedef struct loadInstruction *loadInstruction;
typedef struct generalInstruction *generalInstruction;

loadInstruction 
unpackLoad(uint32_t instruction);

generalInstruction 
unpackGeneral(uint32_t instruction);

void 
unpackInstruction(Seq_T registers, uint32_t instruction);

void 
execute(Seq_T segment_0);

#endif