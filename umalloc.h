// File: umalloc.h

// List all group members' names: Vikram Sahai Saxena (vs799), Vishwas Gowdihalli Mahalingappa (vg421)
// iLab machine tested on: -ilab1.cs.rutgers.edu

#ifndef UMALLOC_H
#define UMALLOC_H

#include <stdio.h>
#include <stdlib.h>

void* umalloc(size_t, char*, int);
void ufree(void*, char*, int);

#define malloc(X) umalloc(X, __FILE__, __LINE__)
#define free(Y) ufree(Y, __FILE__, __LINE__)

/* helper function declarations*/
unsigned int get_allocation_size(size_t);
void set_metadata_values(unsigned char*, unsigned int, unsigned int);
unsigned int get_metadata_type_size(unsigned char*);
unsigned int get_metadata_flag(unsigned char*);
unsigned int get_mem_block_size(unsigned char*);
unsigned int get_request_size(unsigned int);

#endif
