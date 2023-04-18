/*************************************************************
 *
 *                     instructionSet.h
 * 
 *     Assignment: um 
 *     Authors: Angela Shen and Nora A-Rahim
 *     Date: April 14, 2023
 *
 *    This file contains the interface for instructionSet, a module that 
 *    contains the implementations of the instructions of a Universal Machine
 *    that do not involve segments. 
 *    
 **************************************************************/

#ifndef INSTRUCTIONSET_H
#define INSTRUCTIONSET_H

#include <stdio.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include "seq.h"
#include "assert.h"
#include "registers.h"

void add(Seq_T registers, int a, int b, int c);
void multiply(Seq_T registers, int a, int b, int c);
void divide(Seq_T registers, int a, int b, int c);
void conditionalMove(Seq_T registers, int a, int b, int c);
void nand(Seq_T registers, int a, int b, int c);
void input(Seq_T registers, int c);
void output(Seq_T registers, int c);
void loadValue(Seq_T registers, int a, uint32_t value);

#endif