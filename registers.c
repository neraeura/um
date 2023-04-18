/*************************************************************
 *
 *                     registers.c
 * 
 *     Assignment: um 
 *     Authors: Angela Shen and Nora A-Rahim
 *     Date: April 4, 2023
 *
 *    This file contains the implementation for registers, a module that 
 *    simulates the 8 general purpose registers of the Universal Machine. The
 *    registers module is responsible for creating and freeing the registers,
 *    as well as adding values to registers and obtaining the values from a
 *    specified register. 
 *    
 **************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "registers.h"
#include "seq.h"
#include <assert.h>


/************************* createRegisters() ******************************
 *  Purpose: Creates the eight GPRs employed by The Universal Machine and 
 *           initializes each to 0
 *  Parameters: None
 *  Returns: An instance of Seq_T with each register initialized to a uint32_t
 *           of value 0 
 *  Effects: Creates a Hanson sequence of uint32_t's of length 8 
 *  Expects: None
 ****************************************************************************/
Seq_T createRegisters()
{
        /* create a sequence that holds 8 elements */
        Seq_T registers = Seq_new(8);
        assert(registers != NULL);
        uint32_t zero = 0;
        for (int i = 0; i < 8; i++) {
                Seq_addhi(registers, (void *) (uintptr_t) zero);
        }
        return registers;
}

/**************************** getRegister() ****************************
 *  Purpose: Obtains the value within a specified register
 *  Parameters: Seq_T registers: sequence representing all 8 of The Universal 
 *              Machine's General Purpose Registers (GPRs)
 *              int index: the number/index of the register of interest
 *  Returns: a uint32_t representing the value in the specified register  
 *  Effects: Uses Hanson sequence Seq_get function to obtain specific index of 
 *           the sequence representing the 8 GPRs 
 *  Expects: Expects that register exists and the index is between 0-7
 ****************************************************************************/
uint32_t getRegister(Seq_T registers, int index)
{
        assert(registers != NULL);
        return (uint32_t) (uintptr_t) Seq_get(registers, index);
}

/**************************** setRegister() *******************************
 *  Purpose: Assigns a particular value to a specified register
 *  Parameters: An instance of Seq_T, representing all 8 of the UM's GPRs
 *              An int representing the index of a register specified by the
 *              user
 *              An uint32_t representing the value to be assigned to the 
 *              register
 *  Returns: None
 *  Effects: Uses Hanson sequence Seq_put function to set the value of specific 
 *           index of sequence
 *  Expects: Expects that register exists and the index is between 0-7, and 
 *           value can be represented with a uint32_t
 ****************************************************************************/
void setRegister(Seq_T registers, int index, uint32_t value) 
{
        assert(registers != NULL);
        Seq_put(registers, index, (void *) (uintptr_t) value);
}

/**************************** freeRegister() ******************************
 *  Purpose: Frees any memory associated with the creation of the 8 GPRs  
 *           employed by The Universal Machine
 *  Parameters: A reference to Seq_T, representing the 8 GPRs employed by 
 *              The Univeraal Machine
 *  Returns: None
 *  Effects: Uses Hanson sequence function Seq_free
 *  Expects: The sequence to be freed exists
 ****************************************************************************/
void freeRegister(Seq_T *registers)
{       
        assert(registers != NULL);
        assert(*registers != NULL);
        Seq_free(registers);
}

/**************************** printRegister() ****************************
 *  Purpose: Prints the contents of the 8 GPRs employed by the UM
 *  Parameters: An instance of Seq_T, representing the 8 GPRs employed by 
 *              The Universal Machine
 *  Returns: None
 *  Effects: Accesses each register using the getRegister function and prints
 *           to stdout
 *  Expects: The sequence to be printed must exist
 ****************************************************************************/
void printRegister(Seq_T registers)
{
        assert(registers != NULL);
        for (int i = 0; i < 8; i++) {
                uint32_t val = getRegister(registers, i);
                printf("$r[%d] = %u\n", i, val);
        }
}