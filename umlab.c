/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include "assert.h"
#include "seq.h"
#include "bitpack.h"
#include "registers.h"


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        Um_instruction instruction = 0;
        instruction = (uint32_t)Bitpack_newu(instruction, 4, 28, op);   
        instruction = (uint32_t)Bitpack_newu(instruction, 3, 6, ra);
        instruction = (uint32_t)Bitpack_newu(instruction, 3, 3, rb);
        instruction = (uint32_t)Bitpack_newu(instruction, 3, 0, rc);
        
        return instruction;
}

Um_instruction loadval(unsigned ra, unsigned val)
{
        Um_instruction instruction = 0;
        instruction = (uint32_t)Bitpack_newu(instruction, 4, 28, 13);  
        instruction = (uint32_t)Bitpack_newu(instruction, 3, 25, ra);
        instruction = (uint32_t)Bitpack_newu(instruction, 25, 0, val);
        
        return instruction;
}


/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;


Um_instruction cond_move(Um_register a, Um_register b, Um_register c)
{
        return three_register(CMOV, a, b, c);
}

Um_instruction seg_load(Um_register a, Um_register b, Um_register c)
{
        return three_register(SLOAD, a, b, c);
}

Um_instruction seg_store(Um_register a, Um_register b, Um_register c)
{
        return three_register(SSTORE, a, b, c);
}

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction mult(Um_register a, Um_register b, Um_register c) 
{
        return three_register(MUL, a, b, c);
}

static inline Um_instruction divide(Um_register a, Um_register b, 
                                                        Um_register c) 
{
        return three_register(DIV, a, b, c);
}

static inline Um_instruction nand(Um_register a, Um_register b, Um_register c) 
{
        return three_register(NAND, a, b, c);
}

Um_instruction map(Um_register b, Um_register c)
{
        return three_register(ACTIVATE, 0, b, c);
}

Um_instruction unmap(Um_register c)
{
        return three_register(INACTIVATE, 0, 0, c);
}

Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);
}


static inline Um_instruction input(Um_register c) 
{
        return three_register(IN, 0, 0, c);
}


Um_instruction load_program(Um_register b, Um_register c)
{
        return three_register(LOADP, 0, b, c);
}


/* ------------------- Functions for working with streams ----------------- */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                /* writes to file in big-endian order */
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/* -------------------------- Unit tests for the UM ------------------------ */

/* ------------------------------------------------------------------------- */
/*                           CONDITIONAL MOVE TESTS                           */
/* ------------------------------------------------------------------------- */
void conditional_positive(Seq_T stream)
{
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 3));
        append(stream, loadval(r3, 4));
        append(stream, cond_move(r3, r2, r1));
        append(stream, output(r3));
}

void conditional_negative(Seq_T stream)
{
        append(stream, loadval(r1, 0));
        append(stream, loadval(r2, 3));
        append(stream, loadval(r3, 4));
        append(stream, cond_move(r3, r2, r1));
        append(stream, output(r3));
}

void conditional_maximum(Seq_T stream)
{
        append(stream, loadval(r1, 33554431));
        append(stream, loadval(r2, 33554431));
        append(stream, loadval(r3, 4));
        append(stream, cond_move(r3, r2, r1));
        append(stream, output(r3));
}


/* -------------------------------------------------------------------------- */
/*                               SEG LOAD TESTS                               */
/* -------------------------------------------------------------------------- */

void load_segment(Seq_T stream)
{
        append(stream, loadval(r0, 5));
        append(stream, loadval(r7, 20));
        append(stream, loadval(r6, 3));
        append(stream, map(r1, r0));
        append(stream, seg_store(r1, r6, r7));
        append(stream, seg_load(r4, r1, r6));
        append(stream, output(r4));
        append(stream, halt());
}


/* -------------------------------------------------------------------------- */
/*                               SEG STORE TESTS                              */
/* -------------------------------------------------------------------------- */

void seg_storage(Seq_T stream)
{
        append(stream, loadval(r0, 3)); /* load 3 into register 0 */
        append(stream, loadval(r7, 10)); /* load 10 into regitser 7  */
        append(stream, loadval(r6, 80)); /* load 80 or 'P' into register 6 */
        
        append(stream, output(r7)); /* output should be 10  */
        append(stream, loadval(r5, 2)); /* load 2 into register 5 */
       
        append(stream, map(r1, r7)); /*  map segment 1 with 10 words  */
        append(stream, seg_store(r1, r0, r6)); /* expect to see m[1][3] == 80 */

        append(stream, map(r2, r0)); /* map segment 2 with 3 words  */
        append(stream, seg_store(r2, r5, r0)); /* expect to see m[2][2] == 3  */
        append(stream, map(r3, r6)); /* map a 3rd segment with 80 words  */
        append(stream, seg_store(r3, r7, r0)); /* expect to see m[3][10] = 3 */
        append(stream, map(r4, r7)); /* map segment 4 with 10 words */
        append(stream, unmap(r3));
        append(stream, unmap(r1));
        append(stream, map(r3, r0)); 
        append(stream, seg_store(r3, r5, r0)); 
        append(stream, seg_load(r3, r5, r1));
        append(stream, output(r1));
        append(stream, halt());
}

/* -------------------------------------------------------------------------- */
/*                                  ADD TESTS                                 */
/* -------------------------------------------------------------------------- */
void add_commutative(Seq_T stream)
{
        append(stream, loadval(r1, 3));
        append(stream, loadval(r2, 4));
        append(stream, add(r3, r2, r1));
        append(stream, add(r4, r1, r2));
        append(stream, divide(r5, r3, r4));
        append(stream, output(r5));
        append(stream, halt());
}


void add_zeroes(Seq_T stream)
{
        append(stream, loadval(r1, 0));
        append(stream, loadval(r2, 0));
        append(stream, add(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());
}

void add_zero(Seq_T stream)
{
        append(stream, loadval(r1, 0));
        append(stream, loadval(r2, 2));
        append(stream, add(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());
}

void add_to_zero(Seq_T stream)
{
        append(stream, loadval(r1, 2));
        append(stream, loadval(r2, 0));
        append(stream, add(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());

}

void add_overflow(Seq_T stream)
{
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 33554431));
        append(stream, add(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());

}

/* -------------------------------------------------------------------------- */
/*                            MULTIPLICATION TESTS                            */
/* -------------------------------------------------------------------------- */

void multiply_commutative(Seq_T stream)
{
        append(stream, loadval(r1, 3));
        append(stream, loadval(r2, 4));
        append(stream, mult(r3, r2, r1));
        append(stream, mult(r4, r1, r2));
        append(stream, divide(r5, r3, r4));
        append(stream, output(r5));
        append(stream, halt());
}

void multiply_zeroes(Seq_T stream)
{
        append(stream, loadval(r1, 0));
        append(stream, loadval(r2, 0));
        append(stream, mult(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());
}

void multiply_zero(Seq_T stream)
{
        append(stream, loadval(r1, 0));
        append(stream, loadval(r2, 2));
        append(stream, mult(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());
}

void multiply_against_zero(Seq_T stream)
{
        append(stream, loadval(r1, 2));
        append(stream, loadval(r2, 0));
        append(stream, mult(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());
}

/* void multiply_overflow(Seq_T stream)
{
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 33554431));
        append(stream, mult(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());
} */

/* -------------------------------------------------------------------------- */
/*                               DIVISION TESTS                               */
/* -------------------------------------------------------------------------- */
void divide_noncommunative(Seq_T stream)
{
        append(stream, loadval(r1, 4));
        append(stream, loadval(r2, 23));
        append(stream, divide(r3, r2, r1));
        append(stream, divide(r4, r1, r2));
        append(stream, divide(r5, r4, r3));
        append(stream, output(r5));
        append(stream, halt());
}

void divide_against_zero(Seq_T stream)
{
        append(stream, loadval(r1, 2));
        append(stream, loadval(r2, 0));
        append(stream, divide(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());
}

void divide_round_five(Seq_T stream)
{
        append(stream, loadval(r1, 4));
        append(stream, loadval(r2, 23));
        append(stream, divide(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());
}

void divide_round_zero(Seq_T stream)
{
        append(stream, loadval(r1, 23));
        append(stream, loadval(r2, 4));
        append(stream, divide(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());
}

void divide_readable(Seq_T stream)
{
        append(stream, loadval(r1, 2));
        append(stream, loadval(r2, 140));
        append(stream, divide(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());
}

void divide_maximum(Seq_T stream)
{
        append(stream, loadval(r1, 33554431));
        append(stream, loadval(r2, 33554431));
        append(stream, divide(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());
}

/* -------------------------------------------------------------------------- */
/*                                 NAND TESTS                                 */
/* -------------------------------------------------------------------------- */
void nand_o(Seq_T stream)
{
        append(stream, loadval(r2, 0));
        append(stream, nand(r1, r2, r2));
        append(stream, loadval(r7, 99));
        append(stream, add(r0, r1, r7));
        append(stream, output(r0));
        append(stream, halt());
}

/* void nand_invalid(Seq_T stream) 
{
        append(stream, loadval(r2, 0));
        append(stream, nand(r1, r2, r2));
        append(stream, output(r1));
        append(stream, halt());
} */

/* -------------------------------------------------------------------------- */
/*                                 HALT TESTS                                 */
/* -------------------------------------------------------------------------- */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
        
}

/* -------------------------------------------------------------------------- */
/*                                  MAP TESTS                                 */
/* -------------------------------------------------------------------------- */

void map_unmap(Seq_T stream)
{
        append(stream, loadval(r0, 3));
        append(stream, map(r1, r0)); 
        append(stream, map(r2, r0));
        append(stream, map(r3, r0));
        append(stream, map(r4, r0));
        append(stream, map(r5, r0));
        append(stream, map(r6, r0));
        append(stream, map(r7, r0));
        append(stream, unmap(r1)); 
        append(stream, unmap(r2));
        append(stream, unmap(r3));
        append(stream, unmap(r4));
        append(stream, unmap(r5));
        append(stream, unmap(r6));
        append(stream, unmap(r7));
        append(stream, halt());
}

void map_extreme(Seq_T stream)
{
        append(stream, loadval(r0, 65000));
        append(stream, map(r1, r0)); 
        append(stream, map(r2, r0));
        append(stream, map(r3, r0));
        append(stream, map(r4, r0));
        append(stream, map(r5, r0));
        append(stream, map(r6, r0));
        append(stream, map(r7, r0));
        
}

/* -------------------------------------------------------------------------- */
/*                                 UNMAP TESTS                                */
/* -------------------------------------------------------------------------- */

void unmap_seg(Seq_T stream)
{
        append(stream, loadval(r0, 800));
        append(stream, map(r1, r0));
        append(stream, unmap(r1));
        append(stream, output(r1));
        append(stream, halt());
}

void unmap_to_map(Seq_T stream)
{
        /* load 3 into register 0 */
        append(stream, loadval(r0, 3));
        /* make a segment ID 1 with 3 words, seg ID stores in r1  */
        append(stream, map(r1, r0));
        append(stream, map(r2, r0));
        append(stream, map(r3, r0));
        append(stream, map(r4, r0));
        append(stream, unmap(r3));
        append(stream, unmap(r1));
        append(stream, map(r5, r0)); /* reuse segment ID 3  */
        append(stream, map(r6, r0));
        append(stream, halt());
}

/* void unmap_invalid(Seq_T stream) 
{
        append(stream, loadval(r0, 0));
        append(stream, unmap(r0));
        append(stream, halt());
} */

/* -------------------------------------------------------------------------- */
/*                                 INPUT TESTS                                */
/* -------------------------------------------------------------------------- */

void readable_input(Seq_T stream)
{
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, halt());
}

void unreadable_input(Seq_T stream)
{
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, halt());
}

/* void invalid_input(Seq_T stream)
{
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, halt());
} */

/* -------------------------------------------------------------------------- */
/*                                OUTPUT TESTS                                */
/* -------------------------------------------------------------------------- */
void readable_output(Seq_T stream)
{
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 6));
        append(stream, add(r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}

void unreadable_ouput(Seq_T stream)
{
        append(stream, loadval(r1, 32));
        append(stream, loadval(r2, 130));
        append(stream, output(r1));
        append(stream, output(r2));
        append(stream, halt());
}


/* void invalid_output(Seq_T stream)
{
        append(stream, output(r1));
        append(stream, halt());
}
 */

/* -------------------------------------------------------------------------- */
/*                             LOAD PROGRAM TESTS                             */
/* -------------------------------------------------------------------------- */

void load_program_maps(Seq_T stream)
{
        /* ---------------------- LOAD VALUES --------------------------- */
        /* r0 = 0 */
        append(stream, loadval(r0, 0));

         /* r2 = 1 */
        append(stream, loadval(r2, 1));
        
        /* r6 = 11 (HALT) */
        append(stream, loadval(r6, 12));

        /* r5 = 12 (OUTPUT) */
        append(stream, loadval(r5, 13));
        
        /* r4 = 80 */
        append(stream, loadval(r4, 80));

        /* r7 = 1 */
        append(stream, loadval(r7, 2));

        /* ------------------------- MAP ------------------------ */
        /* r1 = s[1] (seg id of 1)*/
        append(stream, map(r1, r7));

        /* ----------------------- OUTPUT SHIT ----------------- */
        /* store s[0][12] into r7 */
        append(stream, seg_load(r7, r0, r5));
        /* s[1][0] = r7 (OUTPUT) */
        append(stream, seg_store(r2, r0, r7));
        
        /* -------------------- HALT SHIT  ------------------------ */
        /* r3 = s[0][11] (HALT) */
        append(stream, seg_load(r3, r0, r6));

        /* s[1][1] = r3 (HALT) */
        append(stream, seg_store(r2, r2, r3));

        /* --------------------- LOAD PROG ------------------- */
        append(stream, load_program(r2, r0));
       
        append(stream, halt());
        append(stream, output(r4));
}



/* -------------------------------------------------------------------------- */
/*                              LOAD VALUE TESTS                              */
/* -------------------------------------------------------------------------- */
void load_value(Seq_T stream)
{
        append(stream, loadval(r1, 80));
        append(stream, output(r1));
        append(stream, halt());
}

void load_minimum(Seq_T stream)
{
        append(stream, loadval(r1, 0));
        append(stream, output(r1));
        append(stream, halt());
}

void load_readable(Seq_T stream)
{
        append(stream, loadval(r1, 66));
        append(stream, output(r1));
        append(stream, halt());
}

void load_addition(Seq_T stream)
{
        append(stream, loadval(r1, 2));
        append(stream, loadval(r2, 3));
        append(stream, loadval(r4, add(r3, r1, r2)));
        append(stream, output(r4));
        append(stream, halt());
}

