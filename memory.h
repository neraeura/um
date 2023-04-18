/*************************************************************
 *
 *                     memory.h
 * 
 *     Assignment: um 
 *     Authors: Angela Shen and Nora A-Rahim
 *     Date: April 14, 2023
 *
 *    This file contains the interface for memory, a module that 
 *    simulates the segmented main memory employed by the Universal Machine. 
 *    The memory module is responsible for mapping, unmapping, and duplicating 
 *    segments, as well as loading values into segments and extracting values 
 *    from segments.
 *    
 **************************************************************/
#ifndef MEMORY_H
#define MEMORY_H


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "seq.h"

Seq_T 
mapSegment(Seq_T registers, Seq_T mapped_segments, Seq_T unmapped_identifiers,
                                                   int rb, int rc);

void 
unmapSegment(Seq_T registers, Seq_T mapped_segments,
             Seq_T unmapped_identifiers, int rc);

void
addSegToMemory(Seq_T mapped_segments, Seq_T segment);

void 
segLoad(Seq_T registers, Seq_T mapped_segments, int ra, int rb, int rc);

void
segStore(Seq_T registers, Seq_T mapped_segments, int ra, int rb, int rc);

void 
loadProgram(Seq_T mapped_segments, Seq_T unmapped_identifiers, Seq_T registers,
            int rb, int rc, uint32_t *program_counter);

int 
segmentLength(Seq_T segment);

Seq_T 
getSegment(Seq_T mapped_segments, uint32_t index);

Seq_T 
duplicateSegment(Seq_T segment);

void
addSegmentIdentifier(Seq_T segment_identifiers, uint32_t identifier);

void
setSegment(Seq_T mapped_segments, uint32_t index, Seq_T segment);

void
setWord(Seq_T segment, uint32_t index, uint32_t word);

uint32_t 
getWord(Seq_T segment, uint32_t index);

void 
printSegment(Seq_T mapped_segments, uint32_t index);

void 
printMemory(Seq_T mapped_segments);

#endif