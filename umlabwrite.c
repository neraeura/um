#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "fmt.h"
#include "seq.h"

extern void Um_write_sequence(FILE *output, Seq_T instructions);

/* ------------------------- CONDITIONAL MOVE TESTS ------------------------- */
extern void conditional_positive(Seq_T stream);
extern void conditional_negative(Seq_T stream);
extern void conditional_maximum(Seq_T stream);

/* --------------------------- SEGMENT LOAD TESTS --------------------------- */
extern void load_segment(Seq_T stream);
/* --------------------------- SEGMENT STORE TESTS -------------------------- */
extern void seg_storage(Seq_T stream);

/* -------------------------------- ADD TESTS ------------------------------- */
extern void add_commutative(Seq_T instructions);
extern void add_zeroes(Seq_T stream);
extern void add_zero(Seq_T stream);
extern void add_to_zero(Seq_T stream);
extern void add_overflow(Seq_T stream);

/* ------------------------------- MULT TESTS ------------------------------- */
extern void multiply_commutative(Seq_T stream);
extern void multiply_zeroes(Seq_T stream);
extern void multiply_zero(Seq_T stream);
extern void multiply_against_zero(Seq_T stream);
extern void multiply_overflow(Seq_T stream);

/* ------------------------------ DIVIDE TESTS ------------------------------ */
extern void divide_noncommunative(Seq_T stream);
extern void divide_against_zero(Seq_T stream);
extern void divide_round_five(Seq_T stream);
extern void divide_round_zero(Seq_T stream);
extern void divide_readable(Seq_T stream);
extern void divide_maximum(Seq_T stream);

/* ------------------------------- NAND TESTS ------------------------------- */
extern void nand_o(Seq_T stream);
extern void nand_invalid(Seq_T stream);

/* ------------------------------- HALT TESTS ------------------------------- */
extern void build_halt_test(Seq_T instructions);
extern void build_verbose_halt_test(Seq_T instructions);

/* -------------------------------- MAP TESTS ------------------------------- */
extern void map_unmap(Seq_T stream);
extern void map_extreme(Seq_T stream);

/* ------------------------------- UNMAP TESTS ------------------------------ */
extern void unmap_seg(Seq_T stream);
extern void unmap_to_map(Seq_T stream);
extern void unmap_invalid(Seq_T stream);

/* ------------------------------- INPUT TESTS ------------------------------ */
extern void readable_input(Seq_T stream);
extern void unreadable_input(Seq_T stream);
extern void invalid_input(Seq_T stream);
/* ------------------------------ OUTPUT TESTS ------------------------------ */
extern void readable_output(Seq_T stream);
extern void unreadable_output(Seq_T stream);
extern void invalid_output(Seq_T stream);

/* --------------------------- LOAD PROGRAM TESTS --------------------------- */
extern void load_program_maps(Seq_T stream);

/* ---------------------------- LOAD VALUE TESTS ---------------------------- */
extern void load_maximum(Seq_T stream);
extern void load_minimum(Seq_T stream);
extern void load_value(Seq_T stream);
extern void load_readable(Seq_T stream);


/* The array `tests` contains all unit tests for the lab. */


static struct test_info {
        const char *name;
        const char *test_input;          /* NULL means no input needed */
        const char *expected_output;
        /* writes instructions into sequence */
        void (*build_test)(Seq_T stream);
} tests[] = {
        /* CONDITIONAL MOVE TESTS */
        {"conditional_positive", NULL, "", conditional_positive },
        {"conditional_negative", NULL, "", conditional_negative },
        {"conditional_maximum", NULL, "", conditional_maximum },
        
        /* SEG LOAD TESTS */
        { "load_segment", NULL, "", load_segment },
        
        
        /* SEG STORE TESTS */
        { "seg_storage", NULL, "", seg_storage },
        
        /* ADD TESTS */
        { "add_commutative", NULL, "", add_commutative },
        { "add_zeroes", NULL, "0", add_zeroes },
        { "add_zero", NULL, "2",  add_zero },
        { "add_to_zero", NULL, "2",  add_to_zero},
        //{ "add_overflow", NULL, "33554432",  add_overflow} ,

        /* MULT TESTS */
        { "multiply_commutative", NULL, "1", multiply_commutative },
        { "multiply_zeroes", NULL, "0", multiply_zeroes },
        { "multiply_zero", NULL, "0", multiply_zero },
        { "multiply_against_zero", NULL, "0", multiply_against_zero },
        //{ "multiply_overflow", NULL, "33554431", multiply_overflow },

        /* DIVIDE TESTS */
        { "divide_noncommunative", NULL, "0", divide_noncommunative },
        { "divide_against_zero", NULL, "0", divide_against_zero },
        { "divide_round_five", NULL, "5", divide_round_five },
        { "divide_round_zero", NULL, "0", divide_round_zero },
        { "divide_readable", NULL, "P", divide_readable }, 
        { "divide_maximum", NULL, "1", divide_maximum },
        // { "divide_invalid", NULL}

        /* NAND TESTS */ // needs expected values
        { "nand_o", NULL, "", nand_o }, 
        // { "nand_invalid", NULL, "", nand_invalid },
        
        /* HALT TESTS */
        { "build_halt_test",         NULL, "", build_halt_test },
        { "build_verbose_halt_test", NULL, "", build_verbose_halt_test },

        // /* MAP TESTS */
        { "map_unmap", NULL, "",  map_unmap},
        { "map_extreme", NULL, "", map_extreme }, 
        
        // /* UNMAP TESTS */
        { "unmap_seg", NULL, "1", unmap_seg },
        { "unmap_to_map", NULL, "", unmap_to_map},
       // { "unmap_invalid", NULL, "", unmap_invalid},

        /* INPUT TESTS */
        { "readable_input", "A", "A", readable_input },
        { "unreadable_input", NULL, "", unreadable_input },
       // { "invalid_input", NULL, "", invalid_input},

        /* OUTPUT TESTS */
        { "readable_output", NULL, "6", readable_output },
        { "unreadable_output", NULL, "6", readable_output },
      // { "invalid_output", NULL, "",  },

        /* LOAD PROGRAM TESTS */
        { "load_program_maps", NULL, "", load_program_maps },
        
        /* LOAD VALUE TESTS */
        { "load_value", NULL, "30", load_value },
        { "load_readable", NULL, "B", load_readable },
        { "load_minimum", NULL, "", load_minimum }

};

  
#define NTESTS (sizeof(tests)/sizeof(tests[0]))

/*
 * open file 'path' for writing, then free the pathname;
 * if anything fails, checked runtime error
 */
static FILE *open_and_free_pathname(char *path);

/*
 * if contents is NULL or empty, remove the given 'path', 
 * otherwise write 'contents' into 'path'.  Either way, free 'path'.
 */
static void write_or_remove_file(char *path, const char *contents);

static void write_test_files(struct test_info *test);


int main (int argc, char *argv[])
{
        bool failed = false;
        if (argc == 1)
                for (unsigned i = 0; i < NTESTS; i++) {
                        printf("***** Writing test '%s'.\n", tests[i].name);
                        write_test_files(&tests[i]);
                }
        else
                for (int j = 1; j < argc; j++) {
                        bool tested = false;
                        for (unsigned i = 0; i < NTESTS; i++)
                                if (!strcmp(tests[i].name, argv[j])) {
                                        tested = true;
                                        write_test_files(&tests[i]);
                                }
                        if (!tested) {
                                failed = true;
                                fprintf(stderr,
                                        "***** No test named %s *****\n",
                                        argv[j]);
                        }
                }
        return failed; /* failed nonzero == exit nonzero == failure */
}


static void write_test_files(struct test_info *test)
{
        FILE *binary = open_and_free_pathname(Fmt_string("%s.um", test->name));
        Seq_T instructions = Seq_new(0);
        test->build_test(instructions);
        Um_write_sequence(binary, instructions);
        Seq_free(&instructions);
        fclose(binary);

        write_or_remove_file(Fmt_string("%s.0", test->name),
                             test->test_input);
        write_or_remove_file(Fmt_string("%s.1", test->name),
                             test->expected_output);
}


static void write_or_remove_file(char *path, const char *contents)
{
        if (contents == NULL || *contents == '\0') {
                remove(path);
        } else {
                FILE *input = fopen(path, "wb");
                assert(input != NULL);

                fputs(contents, input);
                fclose(input);
        }
        free(path);
}


static FILE *open_and_free_pathname(char *path)
{
        FILE *fp = fopen(path, "wb");
        assert(fp != NULL);

        free(path);
        return fp;
}
