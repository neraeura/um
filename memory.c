/******************************************************************************
 *
 *                              memory.c
 * 
 *     Assignment: um 
 *     Authors: Angela Shen and Nora A-Rahim
 *     Date: April 14, 2023
 *
 *    This file contains the implementation for memory, a module that 
 *    simulates the segmented main memory employed by the Universal Machine. 
 *    The memory module is responsible for mapping, unmapping, and duplicating 
 *    segments, as well as loading values into segments and extracting values 
 *    from segments.
 *    
 *****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <assert.h>
#include "memory.h"
#include "registers.h"
#include "seq.h"

 
/**************************** mapSegment() ****************************
 *  Purpose:  Creates a new segment and maps it to an index in memory
 *  Parameters: Seq_T registers: the 8 GPRs employed by the UM 
 *              Seq_T mapped_segments: sequence holding all segments mapped
 *                                     during program execution
 *              Seq_T unmapped_identifiers: sequence holding all segment ID's
 *                                          that were previously used but are
 *                                          now unmapped and are available for
 *                                          reuse
 *              int rc: represents the idx in registers that stores the number  
 *                      of words that will constitute the newly mapped segment
 *              int rb: represents the idx in registers that will store the 
 *                      newly mapped register
 *  Returns: A newly mapped segment represented as a Seq_T
 *  Effects: Allocates memory for a new segment 
 *  Expects: A bit pattern that is not all zeroes and that does not identify 
 *           any currently mapped segment is placed in $r[B]
 *           Sequences must exist and register indices must be within 0-7
 ************************************************************************/
Seq_T mapSegment(Seq_T registers, Seq_T mapped_segments,
                 Seq_T unmapped_identifiers, int rb, int rc)
{       
        /* create a segment */
        Seq_T segment = Seq_new(rc);
        assert(segment != NULL);

        uint32_t rC = getRegister(registers, rc);
        /* initialize elements in segment to 0 */
        uint32_t zero = 0;
        for (uint32_t i = 0; i < rC; i++) {
                Seq_addhi(segment, (void *) (uintptr_t) zero);
        }

        /* add to mapped_segments, reusing an ID from unmapped_seg if
        possible */

        /* if there are available ID's to reuse */
        if (segmentLength(unmapped_identifiers) != 0) {  
                uint32_t free_ID = 
                        (uint32_t) (uintptr_t) Seq_remlo(unmapped_identifiers);
                /* free the previously mapped segment (didn't free in unmap) */
                Seq_T unmap = getSegment(mapped_segments, free_ID);
                Seq_free(&unmap);
                /* store newly_mapped segment */
                setSegment(mapped_segments, free_ID, segment);
                setRegister(registers, rb, free_ID);
        } else { /* if no need to reuse, use any ID */
                addSegToMemory(mapped_segments, segment);
                uint32_t new_ID = segmentLength(mapped_segments) - 1;
                setRegister(registers, rb, new_ID);
        }
        return segment;
}

/**************************** unmapSegment() ****************************
 *  Purpose:  Removes a segment from memory and handles its ID for reuse
 *  Parameters: Seq_T registers: the 8 GPRs employed by the UM 
 *              Seq_T mapped_segments: sequence holding all segments mapped
 *                                     during program execution
 *              Seq_T unmapped_identifiers: sequence holding all segment ID's
 *                                          that were previously used but are
 *                                          now unmapped and are available for
 *                                          reuse
 *              int rc: the segment ID of the segment to be unmapped
 *  Returns: None
 *  Effects: Frees any memory associated with previously mapped segment, and 
 *           makes it's segment ID available for reuse  
 *  Expects: A segment that exists and a valid segment ID, sequences must exist
 *           and register indices must be within 0-7
 ***********************************************************************/
void unmapSegment(Seq_T registers, Seq_T mapped_segments,
                  Seq_T unmapped_identifiers, int rc)
{
        assert(registers != NULL);
        assert(mapped_segments != NULL);
        /* get the segment to unmap from register r[C] */
        uint32_t unmapped_id = getRegister(registers, rc);
        /* add to list of unmapped ID's */
        addSegmentIdentifier(unmapped_identifiers, unmapped_id);    
}


/**************************** segLoad() ****************************
 *  Purpose: Grabs a value from a particular register and loads it into a 
 *           specified word in a specified segment 
 *  Parameters: Seq_T registers: the 8 GPRs employed by the UM 
 *              Seq_T mapped_segments: sequence holding all segments mapped
 *                                     during program execution
 *              int ra: the idx of the register that will take the loaded value
 *              int rb: the idx of the register that contains the segment
 *                      number
 *              int rc: the idx of the register that contains address of the
 *                      word within the segment
 *  Returns: None
 *  Effects: Uses getRegister, getSegment, and getWord to obtain values of
 *           interest, uses register module's set function to modify a
 *           register. 
 *  Expects: Sequences must exist and register indices must be within 0-7
 ***********************************************************************/
void segLoad(Seq_T registers, Seq_T mapped_segments, int ra, int rb, int rc)
{
        assert(registers != NULL);

        /* fetch desired segment from memory */
        uint32_t seg_identifier = getRegister(registers, rb);
        Seq_T desired_segment = getSegment(mapped_segments, seg_identifier);

        /* fetch desired block from that segment */
        uint32_t offset = getRegister(registers, rc);
        uint32_t desired_block = getWord(desired_segment, offset);
        
        /* load into register r[A]*/ 
        setRegister(registers, ra, desired_block);
}

/**************************** segStore() ****************************
 *  Purpose: Stores the value witin a specified register into a particular
 *           word of a particular segment specified by instruction
 *  Parameters: Seq_T registers: the 8 GPRs employed by the UM 
 *              Seq_T mapped_segments: sequence holding all segments mapped
 *                                     during program execution
 *              int ra: the idx of the register that contains the segment
 *                      number
 *              int rb: the idx of the register that contains address of the
 *                      word within the segment
 *              int rc: the idx of the register that contains address of the
 *                      word to be stored in a segment
 *  Returns: None
 *  Effects: Uses getRegister from register module to get the value of a
 *           register, uses getSegment and setWord to obtain values of interest
 *           and set values within a segment
 *  Expects: Sequences must exist and register indices must be within 0-7
 ***********************************************************************/
void segStore(Seq_T registers, Seq_T mapped_segments, int ra, int rb, int rc)
{
        assert(registers != NULL);

        /* fetch desired value from specified register r[C]*/
        uint32_t desired_value = getRegister(registers, rc);

        /* fetch desired ID to store values in from register r[A] */
        uint32_t desired_identifier = getRegister(registers, ra);

        /* use ID to get desired segment to store value in from r[C] */
        Seq_T desired_segment = getSegment(mapped_segments, 
                                           desired_identifier);
        
        /* load value into desired word (offset) at that segment */
        uint32_t rB = getRegister(registers, rb);
        
        setWord(desired_segment, rB, desired_value);

}

/**************************** loadProgram() ****************************
 *  Purpose: Replaces segment-0 with an different specified segment
 *  Parameters: Seq_T mapped_segments: sequence holding all segments mapped
 *                                     during program execution
 *              Seq_T unmapped_identifiers: sequence holding all segment ID's
 *                                          that were previously used but are
 *                                          now unmapped and are available for
 *                                          reuse
 *              Seq_T registers: the 8 GPRs employed by the UM 
 *              int rb: index of the register that contains the segment ID of
 *                      the segment to duplicated and replace segment-0
 *              int rc: index of the register that contains the address of the
 *                      a word the pointer counter will be set to
 *              int *program_counter: pointer to current word
 *  Returns: None
 *  Effects: Uses getRegister to obtain value of a register and uses
 *           getSegment, segmentLength, unmapSegment and setSegment to obtain
 *           and replace segments
 *  Expects: Sequences must exist and register indices must be within 0-7
 ***********************************************************************/
void loadProgram(Seq_T mapped_segments, Seq_T unmapped_identifiers,
                 Seq_T registers, int rb, int rc, uint32_t *program_counter)
{
        (void) unmapped_identifiers;
        
        assert(registers != NULL);
        assert(mapped_segments != NULL);

        uint32_t rB = getRegister(registers, rb);
        uint32_t rC = getRegister(registers, rc);
        
        Seq_T original_segment0 = getSegment(mapped_segments, 0);

        if (rB != 0){
                /* free previous 0-segment */
                Seq_free(&original_segment0);
                
                /* get duplicate segment */
                Seq_T duplicate_segment = 
                        duplicateSegment(getSegment(mapped_segments, rB));
                
                /* set 0-index to duplicate segment */
                setSegment(mapped_segments, 0, duplicate_segment);
        } else {
                assert(rC < (uint32_t) segmentLength(original_segment0));
        }

        *program_counter = rC;
}
/**************************** segmentLength() ****************************
 *  Purpose: Returns the length of a specified segment
 *  Parameters: Seq_T segment: segment whose length is returned 
 *  Returns: An integer representing the length of the specified segment 
 *  Effects: Uses Hanson sequence length function to find length of segment
 *  Expects: segment must exist 
 * *********************************************************************/
int segmentLength(Seq_T segment)
{
        assert(segment != NULL);
        return Seq_length(segment);
}

/**************************** addSegToMemory() ****************************
 *  Purpose: Adds a segment to the collection of every segment mapped in the
 *           program
 *  Parameters: Seq_T mapped_segments: sequence holding all segments mapped
 *                                        during program execution
 *              Seq_T segment: segment to be added to collection of  
 *                             segments during program execution
 *  Returns: None
 *  Effects: Uses Hanson sequence functions to add segment
 *  Expects: segmentIdentifiers and segment must exist 
 * ***********************************************************************/
void addSegToMemory(Seq_T mapped_segments, Seq_T segment)
{
        assert(segment != NULL);
        assert(mapped_segments != NULL);
        Seq_addhi(mapped_segments, segment);
}

/**************************** getSegment() ****************************
 *  Purpose: Grabs a specific segment from a collection of all mapped segments
 *           in memory 
 *  Parameters: Seq_T mapped_segments: sequence holding all segments mapped
 *                                     during program execution
 *              int index: index in the sequence representing the segment to be 
 *                         returned
 *  Returns: A Seq_T to the requested segment from memory
 *  Effects: Uses Hanson sequence functions to get a segment
 *  Expects: mapped_segments must exist and index must be a valid index
 ***********************************************************************/
Seq_T getSegment(Seq_T mapped_segments, uint32_t index)
{
        assert(mapped_segments != NULL);
        return (Seq_T) Seq_get(mapped_segments, index);
}

/************************* addSegmentIdentifier() ****************************
 *  Purpose: Adds a segment identifier to a collection of all reusable segment
 *           ID's
 *  Parameters: Seq_T segment_identifiers: sequence holding all segement ID's
 *                                         that were previously mapped but are
 *                                         now unmapped and able to be reused
 *              uint32_t identifier: a segment ID
 *  Returns: None
 *  Effects: Uses Hanson sequence functions to add to a sequence
 *  Expects: segment_identifiers must exist and the segment with identifier as
 *           its segment ID must be unmapped
 ***********************************************************************/
void addSegmentIdentifier(Seq_T segment_identifiers, uint32_t identifier)
{
        Seq_addhi(segment_identifiers, (void *)(uintptr_t) identifier);
}

/**************************** setSegment() ****************************
 *  Purpose: Adds a segment to a collection of all mapped semgents in memory
 *  Parameters: Seq_T mapped_segments: sequence holding all segments mapped
 *                                     during program execution
 *              int index: the segment ID/index of where the segment is to be
 *                         put in the collection
  *             Seq_T segment: segment to be added to collection of  
 *                             segments during program execution
 *  Returns: None
 *  Effects: Uses Hanson sequence functions to add to a sequence
 *  Expects: mapped_segments & segment must exist and index must be a valid ID
 ***********************************************************************/
void setSegment(Seq_T mapped_segments, uint32_t index, Seq_T segment)
{
        assert(mapped_segments != NULL);
        Seq_put(mapped_segments, index, segment);
}

/**************************** duplicateSegment() ****************************
 *  Purpose: Duplicate a given segment
 *  Parameters: Seq_T segment: segment to be duplicated
 *  Returns: None
 *  Effects: Allocates space for a new segment (sequence)
 *  Expects: segment should exist
 ***********************************************************************/

Seq_T duplicateSegment(Seq_T segment)
{
        int length = Seq_length(segment);
        Seq_T duplicate = Seq_new(length);

        for (int i = 0; i < length; i++) {
                uint32_t currWord = getWord(segment, i);
                Seq_addhi(duplicate, (void *) (uintptr_t) currWord);
        }
        return duplicate;
}

/**************************** getWord() ****************************
 *  Purpose: Grabs a word from a given segment
 *  Parameters: Seq_T segment: segment from which a particular word to be
 *                             retrieved 
 *              int index: index in the segment representing the address 
 *                         of the word to be returned
 *  Returns: The word at the requested index in the specified segment
 *  Effects: Uses Hanson Seq_T functions to obtain a word 
 *  Expects: segment must exist and index must be a valid index
 ***********************************************************************/
uint32_t getWord(Seq_T segment, uint32_t index)
{
       assert(segment != NULL);
       return (uint32_t)(uintptr_t) Seq_get(segment, index);
}

/**************************** setWord() ****************************
 *  Purpose: Puts a word in a given segment
 *  Parameters: Seq_T segment: segment from which a particular word to be
 *                             retrieved 
 *              int index: index of where in the segment the word is to be
 *                         placed
 *              uint32_t word: the word to be placed in the segment
 *  Returns: None
 *  Effects: Uses Hanson Seq_T functions to add a word 
 *  Expects: segment and word must exist and index must be valid
 ***********************************************************************/
void setWord(Seq_T segment, uint32_t index, uint32_t word)
{
        assert(segment != NULL);
        Seq_put(segment, index, (void *) (uintptr_t) word);
}

/**************************** printSegment() ****************************
 *  Purpose: Prints the contents of a specified segment to stdout
 *  Parameters: Seq_T segmentIdentifiers: sequence holding all segments mapped
 *                                        during program execution
 *              int index: index in segmentIdentiifers holding segment to be
 *                         printed
 *  Returns: None
 *  Effects: Prints the contents of a segment to stdout, uses Hanson Seq_T
 *           function to get a segment and uses getWord to extract words
 *           from a segment
 *  Expects: segment must exist
 ***********************************************************************/
void printSegment(Seq_T mapped_segments, uint32_t index)
{
        Seq_T desired_segment = Seq_get(mapped_segments, index);
        for (int i = 0; i < segmentLength(desired_segment); i++) {
                uint32_t contents = getWord(desired_segment, i);
                printf("Contents of m[%d][%d]: %du\n", index, i, contents);
        }
}

/**************************** printMemory() ****************************
 *  Purpose: Prints all the segments mapped in memory 
 *  Parameters: Seq_T segmentIdentifiers: sequence holding all segments 
 *                                        mapped during program execution
 *  Returns: None
 *  Effects: Uses Hanson Seq_T functions to obtain a segment, prints data to 
 *           stdout, uses segmentLength and getWord to get values of interest
 *  Expects: segmentIdentifiers must exist and index must be a valid index
 ***********************************************************************/
void printMemory(Seq_T mapped_segments)
{
       assert(mapped_segments != NULL);

       for (int outer = 0; outer < segmentLength(mapped_segments); outer++){
                if(outer == 0)
                {
                        printf("Segment 0 has the value 3 in it\n");
                        outer++;
                }
                Seq_T desired_segment = Seq_get(mapped_segments, outer);
                printf("-------- Segment %d: ---------\n", outer);
                for (int inner = 0; inner < segmentLength(desired_segment); 
                                                                inner++) {

                        uint32_t contents = getWord(desired_segment, inner);
                        printf("Contents of m[%d][%d]: %u\n", outer, inner, 
                                                                contents);
                }
       }
       printf("DONE PRINTING MEMORY\n");
}
