/*************************************************************
 *
 *                     registers.h
 * 
 *     Assignment: um 
 *     Authors: Angela Shen and Nora A-Rahim
 *     Date: April 4, 2023
 *
 *    This file contains the interface for registers, a module that 
 *    simulates the 8 general purpose registers of the Universal Machine. The
 *    registers module is responsible for creating and freeing the registers,
 *    as well as adding values to registers and obtaining the values from a
 *    specified register. 
 *    
 **************************************************************/

#ifndef REGISTERS_H
#define REGISTERS_H

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "seq.h"

Seq_T
createRegisters();

uint32_t 
getRegister(Seq_T registers, int index);

void 
setRegister(Seq_T registers, int index, uint32_t value); 

void 
freeRegister(Seq_T *registers);

void 
printRegister(Seq_T registers);

#endif