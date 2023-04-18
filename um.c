#include <stdlib.h>
#include <stdio.h>
#include "seq.h"
#include "assert.h"
#include "registers.h"
#include <sys/stat.h>
#include "fetcher.h"
#include "executor.h"

const int WORD_SIZE = 4;

int main(int argc, char *argv[])
{
        /* check for proper number of command line arguments */
        if (argc != 2) {
                fprintf(stderr, "Usage: ./um [UM binary filename]\n");
                exit(EXIT_FAILURE);
        }

        /* check that the file exists  */
        struct stat file;
        if (stat(argv[1], &file) == -1) {
                fprintf(stderr, "No such file or directory\n");
                exit(EXIT_FAILURE);
        }

        /* using file size, compute number of instructions */
        int program_size = file.st_size / WORD_SIZE;

        /* create a segment big enough to hold all instructions */
        Seq_T segment_0 = Seq_new(program_size);
        
        /* load program instructions into segment-0 */
        loadProgramInstructions(argv[1], segment_0, program_size);

        /* execute each instruction */
        execute(segment_0);

        return EXIT_SUCCESS;
}