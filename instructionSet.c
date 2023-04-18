/*************************************************************
 *
 *                     instructionSet.c
 * 
 *     Assignment: um 
 *     Authors: Angela Shen and Nora A-Rahim
 *     Date: April 14, 2023
 *
 *    This file contains the implementation for instructionSet, a module that 
 *    contains the implementations of the instructions of a Universal Machine
 *    that do not involve segments. 
 *    
 **************************************************************/

#include <stdio.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include "seq.h"
#include "assert.h"
#include "registers.h"

/**************************** add() ****************************
 *  Purpose: Add values in two registers and store in another
 *  Parameters: Seq_T registers: the 8 GPRs employed by the UM 
 *              int a: index of the register the sum will be stored in
 *              int b: index of the register the first value is in
 *              int c: index of the register the second value is in
 *  Returns: None
 *  Effects: Uses register module functions to access and modify registers
 *  Expects: registers must exist and a, b, c must be within 0-7
 ***********************************************************************/
void add(Seq_T registers, int a, int b, int c)
{
        uint32_t result = getRegister(registers, b) + 
                          getRegister(registers, c);
        result = result;
        setRegister(registers, a, result);
}

/**************************** multiply() ****************************
 *  Purpose: Multiply values in two registers and store in another
 *  Parameters: Seq_T registers: the 8 GPRs employed by the UM 
 *              int a: index of the register the product will be stored in
 *              int b: index of the register the first value is in
 *              int c: index of the register the second value is in
 *  Returns: None
 *  Effects: Uses register module functions to access and modify registers
 *  Expects: registers must exist and a, b, c must be within 0-7
 ***********************************************************************/
void multiply(Seq_T registers, int a, int b, int c)
{
        uint32_t product = getRegister(registers, b) * 
                           getRegister(registers, c);
        product = product;
        setRegister(registers, a, product);
}

/**************************** divide() ****************************
 *  Purpose: Divide values in two registers and store in another
 *  Parameters: Seq_T registers: the 8 GPRs employed by the UM 
 *              int a: index of the register the quotient will be stored in
 *              int b: index of the register the dividend is in
 *              int c: index of the register the divisor is in
 *  Returns: None
 *  Effects: Uses register module functions to access and modify registers
 *  Expects: registers must exist and a, b, c must be within 0-7
 ***********************************************************************/
void divide(Seq_T registers, int a, int b, int c)
{
        /* implement CRE dividing by 0 */
        assert(getRegister(registers, c) != 0); 
        uint32_t quotient = getRegister(registers, b) / 
                            getRegister(registers, c);
        setRegister(registers, a, quotient);
        
}

/**************************** conditionalMove() ****************************
 *  Purpose: Set one register's value to another if the other register's value
 *           is zero
 *  Parameters: Seq_T registers: the 8 GPRs employed by the UM 
 *              int a: index of register to be given new value
 *              int b: index of register with value to be moved
 *              int c: index of register that contains the condition
 *  Returns: None
 *  Effects: Uses register module functions to access and modify registers
 *  Expects: registers must exist and a, b, c must be within 0-7
 ***********************************************************************/
void conditionalMove(Seq_T registers, int a, int b, int c)
{
        bool c_is_0 = true;
        if (getRegister(registers, c) != 0) {
                c_is_0 = false;
        }
        if (!c_is_0)  {
                uint32_t b_value = getRegister(registers, b);
                setRegister(registers, a, b_value);
        }
}

/**************************** nand() ****************************
 *  Purpose: Store the bitwise nand value of two values in registers into
 *           another register
 *  Parameters: Seq_T registers: the 8 GPRs employed by the UM 
 *              int a: index of register that has the 1st value to be nand'ed
 *              int b: index of register that has the 2nd value to be nand'ed
 *              int c: index of the register that the result will be stored
 *  Returns: None
 *  Effects: Uses register module functions to access and modify registers
 *  Expects: registers must exist and a, b, c must be within 0-7
 ***********************************************************************/
void nand(Seq_T registers, int a, int b, int c)
{
        uint32_t rB = getRegister(registers, b);
        uint32_t rC = getRegister(registers, c);
        uint32_t nand = ~(rB & rC);
        setRegister(registers, a, nand);
}

/**************************** input() ****************************
 *  Purpose: Character inputted by user is loaded into a register, if the end
 *          
 *  Parameters: Seq_T registers: the 8 GPRs employed by the UM 
 *              int c: the index of the register the value is loaded into
 *  Returns: None
 *  Effects: Uses register module to set value of a register
 *  Expects: registers must exist, c must be within 0-7, inputted value must
 *           be between 0 and 255
 ***********************************************************************/
void input(Seq_T registers, int c)
{
        int input = getchar();
        uint32_t sentinel = ~0;
        if (input != EOF) {
                setRegister(registers, c, input);
        } else {
                setRegister(registers, c, sentinel);   
        }
}

/**************************** output() ****************************
 *  Purpose: Outputs the the value in a register as a character
 *  Parameters: Seq_T registers: the 8 GPRs employed by the UM 
 *              int c: the index of the register containing the character
 *  Returns: None
 *  Effects: Uses register module to get value of a register
 *  Expects: registers must exist and a, b, c must be within 0-7
 ***********************************************************************/
void output(Seq_T registers, int c)
{
        uint32_t c_val = getRegister(registers, c);
        assert(c_val <= 255);
        printf("%c", c_val);
}

/**************************** loadValue() ****************************
 *  Purpose: Load a value into a specified register
 *  Parameters: Seq_T registers: the 8 GPRs employed by the UM 
 *              int a: the index of the register to be modified
 *              int value: the value to be put in the register
 *  Returns: None
 *  Effects: Uses register module to set value of a register
 *  Expects: registers must exist and a, b, c must be within 0-7
 ***********************************************************************/
void loadValue(Seq_T registers, int a, uint32_t value)
{
        setRegister(registers, a, value);
}



