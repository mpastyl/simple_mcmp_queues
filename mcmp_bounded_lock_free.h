#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <omp.h>
//#include "include/atomic_ops_if.h"
////#include "include/lock_if.h"

#define DEBUG 1
#if DEBUG 
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...) do {} while (0)
#endif


void init_queue(uint32_t size);
int enqueue(uint32_t data);
int dequeue(uint32_t *data);
void print_size();

// Barriers
#  define COMPILER_BARRIER() asm volatile ("" ::: "memory")
#  define COMPILER_NO_REORDER(exec)		\
  COMPILER_BARRIER();				\
  exec;						\
  COMPILER_BARRIER()
#define MB() asm volatile ("mfence" : :)
#define WMB() asm volatile ("sfence" : :)//__asm__ __volatile__ ("" : : : "memory")
#define RMB() asm volatile ("lfence" : :)//MB()
