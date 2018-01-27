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


// Barriers
#  define COMPILER_BARRIER() asm volatile ("" ::: "memory")
#  define COMPILER_NO_REORDER(exec)		\
  COMPILER_BARRIER();				\
  exec;						\
  COMPILER_BARRIER()
#define MB() asm volatile ("mfence" : :)
#define WMB() asm volatile ("sfence" : :)//__asm__ __volatile__ ("" : : : "memory")
#define RMB() asm volatile ("lfence" : :)//MB()


//Lock implementation
#  define TTAS_FREE 0
#  define TTAS_LCKD 1


static inline void ttas_init(volatile uint32_t* l)
{
  *l = TTAS_FREE;
}

static inline uint32_t ttas_lock(volatile uint32_t * l)
{
  while (1)
    {
      while (*l == TTAS_LCKD)
	{
	  asm volatile ("nop");
	}

      if (__sync_val_compare_and_swap(l, TTAS_FREE, TTAS_LCKD) == TTAS_FREE)
      //if (__sync_lock_test_and_set(l, TTAS_LCKD) == TTAS_FREE)
	  //FIXME: try with sync_lock_test_and_set
	{
	  break;
	}
    }

  return 0;
}

static inline uint32_t ttas_unlock(volatile uint32_t* l)
{
  COMPILER_NO_REORDER(*l = TTAS_FREE;);
  return 0;
}
