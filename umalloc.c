// File: umalloc.c

// Authors: Vikram Sahai Saxena (vs799), Vishwas Gowdihalli Mahalingappa (vg421)
// iLab machine tested on: -ilab1.cs.rutgers.edu

#include "umalloc.h"

//10 MB
#define MEM_BYTE_SIZE 10*1024*1024

//Flag value in meta data to indicate reserved memory space
#define MEMORY_RESERVED 1

//Flag value in meta data to indicate free memory space
#define MEMORY_FREE 0

//static char memory array
static char mem[MEM_BYTE_SIZE];

//memory initialization variable with default value = 'n'
char init = 'n';

/**
 * User level implementation of malloc()
 * 
 * @param request_bytes : requested byte size
 * @param file_ptr : pointer to the file that calls malloc()
 * @param line_num : line number where umalloc() is called
 * @return void* : pointer to the location in mem array, where we can start writing
 */
void* umalloc(size_t request_bytes, char* file_ptr, int line_num) {
	void* umalloc_ptr = NULL; //pointer returned by umalloc()
    unsigned char* mem_ptr = NULL; //pointer to mem array
    unsigned int bytes_for_allocation = get_allocation_size(request_bytes); //allocation size required to allocate memory block

    //check for errors on umalloc()
    if (MEM_BYTE_SIZE < bytes_for_allocation) {
        printf("Error on malloc(): there is no free memory to allocate in line %d, %s\n", line_num, file_ptr);
        return umalloc_ptr;
    }   
    if (0 >= (long)request_bytes) {
        printf("Error on malloc(): invalid argument passed to malloc() in line %d, %s\n", line_num, file_ptr);
        return umalloc_ptr;
    }

    //memory block allocation
    if ('n' == init) {//if umalloc() has not been called once
        mem_ptr = (unsigned char*)&mem[0];
        set_metadata_values(mem_ptr, bytes_for_allocation, MEMORY_RESERVED); //allocate memory block and set the metadata values
        init = 'i'; //mem has been initialized
        umalloc_ptr = mem_ptr + get_metadata_type_size(mem_ptr); //pointer to the location in mem array, where we can start writing 
        set_metadata_values(mem_ptr + bytes_for_allocation, 0, MEMORY_FREE); //identifying the next free position in mem after the allocated memory block
        return umalloc_ptr;
    } else {//mem has been initialized
        unsigned int mem_index = 0; //mem array index
        unsigned int stop_mem_block_search = 0; //flag to stop checking mem space for umalloc()
        while (0 == stop_mem_block_search) {//check mem space until flag = 1
            mem_ptr = (unsigned char*)&mem[mem_index];
            if (MEMORY_FREE == get_metadata_flag(mem_ptr)) {//the memory block pointed by mem_ptr is free
                unsigned int mem_block_size = get_mem_block_size(mem_ptr); //get the memory block size stored in the metadata pointed by mem_ptr
                int remaining_mem_block_size = mem_block_size - bytes_for_allocation; //remaining mem size that can be left free after allocation in current mem block
                if (0 < mem_block_size) {
                    if (0 <= (remaining_mem_block_size)) { 
                        set_metadata_values(mem_ptr, bytes_for_allocation, MEMORY_RESERVED); //allocate memory block and set the metadata values
                        umalloc_ptr = mem_ptr + get_metadata_type_size(mem_ptr); //pointer to the location in mem array, where we can start writing
                        if (0 < remaining_mem_block_size) { // we can possibly allocate a mem block in this free region, so we need to identify it by setting a metadata with MEMORY_FREE flag
                            set_metadata_values(mem_ptr + bytes_for_allocation, remaining_mem_block_size, MEMORY_FREE); //identifying the remaining free space in mem array
                        }
                        return umalloc_ptr;
                    }
                } else {
                    //check if memory allocation is possible
                    if (0 <= (MEM_BYTE_SIZE - bytes_for_allocation - mem_index)) { //allocation is possible
                        set_metadata_values(mem_ptr, bytes_for_allocation, MEMORY_RESERVED); //allocate memory block and set the metadata values
                        umalloc_ptr = mem_ptr + get_metadata_type_size(mem_ptr); //pointer to the location in mem array, where we can start writing
                        set_metadata_values(mem_ptr + bytes_for_allocation, 0, MEMORY_FREE); //identifying the next free position in mem after the allocated memory block
                        return umalloc_ptr;
                    } else { //allocation is not possible
                        printf("Error on malloc(): memory is not full but there is not enough free memory for allocation in line %d, %s\n", line_num, file_ptr);
                        return umalloc_ptr;
                    }
                }
            }
            if (MEM_BYTE_SIZE > mem_index) {//we can continue checking free space for umalloc()
                mem_index += get_mem_block_size(mem_ptr); //go to the index next to the current memory block
            } else {
                stop_mem_block_search = 1; //stop checking free space for umalloc()
            }
        }
    }
    printf("Error on malloc(): there is no free memory to allocate in line %d, %s\n", line_num, file_ptr); //Unsuccessful umalloc()
    return umalloc_ptr;
}

/**
 * User level implementation of free()
 * 
 * @param ptr : pointer to the memory location that we want to free
 * @param file_ptr : pointer to the file that calls ufree()
 * @param line_num : line number where ufree() is called
 */
void ufree(void* ptr, char* file_ptr, int line_num) {
    
    //check for errors on ufree()
    if (NULL == ptr) {
        printf("Error on free(): attempted to free a NULL pointer in line %d, %s\n", line_num, file_ptr);
        return;
    }
    if (8 != sizeof(ptr)) {
        printf("Error on free(): argument passed to free() is not a pointer in line %d, %s\n", line_num, file_ptr);
        return;
    }
   
    unsigned int mem_index = 0;//mem array index
    unsigned char *free_mem_ptr = NULL;//pointer to mem array
    unsigned short ptr_freed = 0;// flag to indicate ptr was successfully freed
    
    //freeing memory
    while (MEM_BYTE_SIZE > mem_index) {
        unsigned char* mem_ptr = (unsigned char*)&mem[mem_index];
        if (MEMORY_RESERVED == get_metadata_flag(mem_ptr)) {//the memory block pointed by mem_ptr is allocated/reserved
            if (ptr != (mem_ptr + get_metadata_type_size(mem_ptr))) {// pointer to the memory location that we want to free and mem_ptr do not match
                free_mem_ptr = NULL;//could not identify free region 
                mem_index += get_mem_block_size(mem_ptr);
            } else {// pointer to the memory location that we want to free and mem_ptr match
                ptr_freed = 1;//ptr is freed
                if (0 != get_request_size(get_mem_block_size(mem_ptr))) {//set the mem block as free if the request size in mem block is not zero
                   set_metadata_values(mem_ptr, get_mem_block_size(mem_ptr), MEMORY_FREE);
                }
            }
        } else {//the memory block pointed by mem_ptr is free
            unsigned int mem_block_size = get_mem_block_size(mem_ptr);
            if (0 < mem_block_size) {
                //if pointer to the memory location that we want to free was already freed and it matches with ptr, then redundant free
                if (0 == ptr_freed && (ptr == (mem_ptr + get_metadata_type_size(mem_ptr)))) {
                    ptr_freed = 1;
                    printf("Error on free(): redundant free of pointer in line %d, %s\n", line_num, file_ptr);
                    break;
                }
                //merge adjacent free blocks and set the merged mem block metadata flag as MEMORY_FREE 
                if (NULL != free_mem_ptr) {
                    int merged_free_block_size = get_mem_block_size(free_mem_ptr) + get_mem_block_size(mem_ptr); //merged free block size
                    set_metadata_values(free_mem_ptr, merged_free_block_size, MEMORY_FREE);
                }
                mem_index += get_mem_block_size(mem_ptr); //move to the next mem index after current memory block
            } else {//there is no space available for allocation/freeing
                if (NULL != free_mem_ptr) {
                    set_metadata_values(free_mem_ptr, 0, MEMORY_FREE);
                }
                break;
            }
            if (NULL == free_mem_ptr) {//make the current mem_ptr as the free pointer
                free_mem_ptr = mem_ptr;
            }
        }
    }
    if (0 == ptr_freed) {
        printf("Error on free(): attempted to free pointer not allocated by malloc() in line %d, %s\n", line_num, file_ptr);//unsuccessful ufree()
    }
}

/**
 * Get the allocation size required to allocate memory block
 * 
 * @param request_bytes : requested byte size
 * @return unsigned int : size required to allocate memory block = requested byte size + metadata byte size
 */
unsigned int get_allocation_size(size_t request_bytes) {
	if (31 >= request_bytes + 1) {
        return request_bytes + 1;
    } else if (8191 >= request_bytes + 2) {
        return request_bytes + 2;
    } else {
		return request_bytes + 4;
	}
}

/**
 * Set the metadata values at location pointed by mem pointer
 * 
 * @param mem_ptr : pointer to mem array
 * @param bytes_for_allocation : the allocation size required to allocate memory block
 * @param metadata_flag : the metadata flag value, indicating reserved space or free space
 */
void set_metadata_values(unsigned char* mem_ptr, unsigned int bytes_for_allocation, unsigned int metadata_flag) {
    if (31 >= bytes_for_allocation) {
        *mem_ptr = (bytes_for_allocation << 3) + metadata_flag;
    } else if (8191 >= bytes_for_allocation) {
        *((unsigned short*)mem_ptr) = (bytes_for_allocation << 3) + metadata_flag + 2;
    } else {
		*((unsigned int*)mem_ptr) = (bytes_for_allocation << 3) + metadata_flag + 4;
	}
}

/**
 * Get the byte size required for metadata
 * 
 * @param mem_ptr : pointer to mem array
 * @return unsigned int : 1 for unsigned char metadata, 2 for unsigned short metadata, 4 for unsigned int metadata
 */
unsigned int get_metadata_type_size(unsigned char* mem_ptr) {
	if (0 == ((*mem_ptr >> 1) & 3)) {
		return 1;
	} else if (1 == ((*mem_ptr >> 1) & 3)) {
		return 2;
	} else {
		return 4;
	}
}

/**
 * Get the metadata flag value, indicating reserved space or free space
 * 
 * @param mem_ptr : pointer to mem array
 * @return unsigned int : 1 for reserved space, 0 for free space
 */
unsigned int get_metadata_flag(unsigned char* mem_ptr) {
    return *mem_ptr & 1;
}

/**
 * Get the memory block size value stored in metadata
 * 
 * @param mem_ptr : pointer to mem array
 * @return unsigned int : memory block size = requested byte size + metadata byte size
 */
unsigned int get_mem_block_size(unsigned char* mem_ptr) {
    unsigned int metadata_type_size = get_metadata_type_size(mem_ptr);
    if (1 == metadata_type_size) {
        return (*mem_ptr >> 3);
    } else if (2 == metadata_type_size) {
        return (*(unsigned short*)mem_ptr) >> 3;
    } else {
		return (*(unsigned int*)mem_ptr) >> 3;
	}
}

/**
 * Get the request size byte value from memory block size value
 * 
 * @param mem_block_size : memory block size value
 * @return unsigned int : request size byte value
 */
unsigned int get_request_size(unsigned int mem_block_size) {
    if (31 >= mem_block_size) {
        return mem_block_size - 1;
    } else if (8191 >= mem_block_size) {
        return mem_block_size - 2;
    } else {
		return mem_block_size - 4;
	}
}


