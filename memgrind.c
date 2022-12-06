/**
 * File: memgrind.c
 * Authors: Vikram Sahai Saxena(vs799), Vishwas Gowdihalli Mahalingappa(vg421)
 * iLab machine tested on: -ilab1.cs.rutgers.edu
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/time.h>
#include "umalloc.h"

/**
 * Global variable used for indexing the array
 */
unsigned int j = 0;

/**
 * Global character array to store the pointers of allocated memory space.
 */
unsigned char * store[524288];


/**
 * Testcase1: Consistency.
 * Allocate a small block (1 to 10B), cast it to a type write to it then free it.
 * Allocate a block of the same size, cast it to the same type, then check to see if the address of the pointers is the same.
 */
void consistency() 
{
        void * four = malloc(4);
        char * cast = (char * ) four;
        //printf("%p\n",four );
        free(cast);

        void * four2 = malloc(4);
        char * cast2 = (char * ) four2;

        if (cast == cast2) 
        {
                printf("consistency testing passed\n");
                printf("First pointer %p\n", four);
                printf("Second pointer %p\n\n", four2);
                free(four2);
        } 
        else 
        {
                printf("consistency testing failed\n\n");
        }
}


/**
 * Testcase2: Maximization.
 * Allocate 1B, if the result is not NULL, free it, double the size and try again.
 * On NULL, halve the size and try again.
 * On success after a NULL, stop, free it.
 * This method checks for maximization testcase and return the maxmemory.
 */
unsigned int maximization() 
{
        unsigned int memory = 1;
        unsigned int maxmemory = 0;
        //printf("start\n");
        void * one = malloc(memory);
        while (NULL != one) 
        {
                //printf("while1\n");
                free(one);
                memory = memory * 2;
                one = malloc(memory);
                //printf("max %d\n",memory);
                //printf("%p\n",one );
        }

        if (NULL == one) 
        {
                printf("Error to allocate %d memory so halve memory and trying again\n\n", memory);
                //printf("inside null\n");
                maxmemory = memory / 2; // Used to store maxmemory;
                //printf("max %d\n",maxmemory);
                //one = malloc(memory/2);
                while (NULL == one) 
                {
                        //printf("while2\n");
                        memory = memory / 2;
                        one = malloc(memory);
                        //printf("%p\n",one );
                }
                if (NULL != one) 
                {
                        printf("maximization testing passed \n");
                        free(one);
                } 
                else 
                {
                        printf("maximization testing failed \n");
                }
        }
        return maxmemory;
}


/**
 * Testcase3: Basic Coalescence.
 * This method takes maxmemory obtained by the previous testcase as the parameter.
 * Allocate one half of maximal allocation.
 * Allocate one quarter of maximal allocation.
 * Free the first pointer.
 * Free the second pointer.
 * Attempt to allocate maximal allocation.
 * On success free it.
 */
void BasicCoalescence(unsigned int maxmemory) 
{
        void * half = malloc(maxmemory / 2);
        //printf("%p\n",half );
        void * quarter = malloc(maxmemory / 4);
        //printf("%p\n",quarter );
        free(half);
        free(quarter);
        void * max = malloc(maxmemory);
        if (NULL == max) 
        {
                printf("BasicCoalescence testing failed \n\n");
        } 
        else 
        {
                printf("BasicCoalsencence testing passed \n\n");
        }

        //printf("%p\n",max );
        free(max);
        //printf("after free\n" );
        //printf("%p\n",max );
}


/**
 * Testcase4: Saturation.
 * Do 9K 1KB allocations (i.e., do 9216 1024-byte allocations).
 * Switch to 1B allocations until malloc responds with NULL, that is the saturation of space.
 * This method gives us the saturation point of our memory.
 */
void * saturation(unsigned int maxmemory) 
{
        unsigned int counter = 0;
        struct timeval begin, end;
        void * s;
        void * saturation;
        printf("Before 1st for loop\n");

        void * maxalloc = malloc(maxmemory);
        if (NULL != maxalloc) 
        {
                printf("In saturation max is passed \n");
                free(maxalloc);
        } 
        else 
        {
                printf("In saturation max is failed \n");
        }
        gettimeofday( & begin, 0);
        for (unsigned int i = 0; i < 9216; i++) 
        {
                s = malloc(1024);
                store[j++] = s;
        }
       

        printf("before 2nd for loop\n");

        for (unsigned int i = 0; NULL != (s = malloc(1)); i++) 
        {
                saturation = s;
                store[j++] = s;
                counter++;
        }
        gettimeofday( & end, 0);
        printf("counter value %d\n\n", counter);
        long seconds = end.tv_sec - begin.tv_sec;
        printf("seconds = %ld \n", seconds);
        return saturation;

}


/**
 * Testcase5: Time Overhead.
 * Saturate the memory (obtained from above test case).
 * Free the last 1B block.
 * Get the current time.
 * Allocate 1B.
 * Get the current time, compute the elapsed time, that will be the max time overhead.
 * This method gives the Max time overhead in microseconds.
 */
void * TimeOverhead(void * saturationPoint) 
{
        struct timeval begin, end;
        void * saturationpoint2;
        free(saturationPoint);
        gettimeofday( & begin, 0);
        saturationpoint2 = malloc(1);
        gettimeofday( & end, 0);
        long timeOverhead = end.tv_usec - begin.tv_usec;
        printf("max time overhead in microseconds = %ld \n", timeOverhead);
        return saturationpoint2;
}


/**
 * Testcase6: Intermediate Coalescence.
 * Saturate the memory (obtained from saturation test case).
 * Free each allocation, one by one.
 * Attempt to allocate your maximal allocation.
 * On success free all memory.
 */
void IntermediateCoalescence(void * saturationPoint2, unsigned int maxmemory) 
{
        struct timeval begin, end;
        gettimeofday( & begin, 0);
        for (int i = 0; i < 524288; i++) 
        {
                free(store[i]);
        }
        gettimeofday( & end, 0);
        long seconds = end.tv_sec - begin.tv_sec;
        printf("seconds in IntermediateCoalescence  = %ld \n", seconds);

        void * maxalloc = malloc(maxmemory);
        if (NULL != maxalloc) 
        {
                printf("Intermediate Coalescence test passed \n");
                free(maxalloc);
        } 
        else 
        {
                printf("Intermediate Coalescence test failed \n");
        }
}


int main() 
{

        consistency();
        unsigned int maxmemory = maximization();
        printf("maximum value %d\n\n", maxmemory);

        BasicCoalescence(maxmemory);
        void * saturationPoint = saturation(maxmemory);
        printf("maximum saturation point %p\n\n", saturationPoint);

        void * saturationPoint2 = TimeOverhead(saturationPoint);
        //printf("maximum value2 %p\n\n",saturationPoint2);
        printf("maximum memory %d\n\n", maxmemory);
        IntermediateCoalescence(saturationPoint2, maxmemory);
        //printf("counter value %d\n\n",counter);
}