#include "mcmp_bounded_lock_based.h"

uint32_t * buffer;
volatile uint32_t head;
volatile uint32_t tail;
uint32_t mask;
uint32_t N;
volatile uint32_t enq_lock;
volatile uint32_t deq_lock;
//TODO: padd me

void init_queue(uint32_t size){
	
	N = size;
	head = 0;
	tail = 0;
	mask = N-1;
    PRINTF(" just before i init the lock\n");
	ttas_init(&enq_lock);
	ttas_init(&deq_lock);
	PRINTF(" lock initialized\n");
	assert((N & mask)==0 && "Size of buffer was not a power of 2");
	buffer = (uint32_t *) calloc(N, sizeof(uint32_t));
	
}

int enqueue(uint32_t data){
		
	//lock me
	int full=0;
	ttas_lock(&enq_lock);
	uint32_t diff  = head - tail;
	if (diff<N){
		buffer[head & mask] = data;
		head++;
	}
	else full=1;
	ttas_unlock(&enq_lock);
	//unlock me
	return full;
}

int dequeue(uint32_t * data){

	int empty=0;
	uint32_t ret;
	//lock me 
	ttas_lock(&deq_lock);
	uint32_t diff = head - tail;
	if (diff>0){
		ret = buffer[tail & mask];
		*data=ret;
		tail++;
	}
	else empty=1;
	//unlock me
	ttas_unlock(&deq_lock);
	return empty;
} 
