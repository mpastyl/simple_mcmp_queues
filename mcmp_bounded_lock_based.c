#include "mcmp_bounded_lock_based.h"

typedef struct node_t
{
	uint32_t value;
	uint8_t pad[64-sizeof(uint32_t)];
}node_t;

typedef __attribute__ ((aligned (64))) struct buf_t
{
	volatile uint32_t head;
	uint8_t pad1[64 - sizeof(uint32_t)];
	
	volatile uint32_t tail;
	uint8_t pad2[64 - sizeof(uint32_t)];
	
	uint32_t mask;
	uint32_t N;
	uint8_t pad3[64 - 2*sizeof(uint32_t)];
	
	volatile uint32_t enq_lock;
	uint8_t pad4[64 - sizeof(uint32_t)];
	
	volatile uint32_t deq_lock;
	uint8_t pad5[64 - sizeof(uint32_t)];
	node_t * array;
}buf_t;

struct buf_t buffer;

void init_queue(uint32_t size){
	
	buffer.N = size;
	buffer.head = size/2; //populate half the queue
	buffer.tail = 0;
	buffer.mask = buffer.N-1;
	ttas_init(&buffer.enq_lock);
	ttas_init(&buffer.deq_lock);
	assert((buffer.N & buffer.mask)==0 && "Size of buffer was not a power of 2");
	buffer.array = (node_t *) calloc(buffer.N, sizeof(node_t));
}

void print_size(){

	printf("Number of elements in the buffer %d\n",buffer.head-buffer.tail);
	printf("head is %d tail is %d\n",buffer.head,buffer.tail);
}

int enqueue(uint32_t data){
		
	//lock me
	int full=0;
	ttas_lock(&buffer.enq_lock);
	uint32_t diff  = buffer.head - buffer.tail;
	if (diff<buffer.N){
		buffer.array[buffer.head & buffer.mask].value = data;
		buffer.head++;
	}
	else full=1;
	ttas_unlock(&buffer.enq_lock);
	//unlock me
	return full;
}

int dequeue(uint32_t * data){

	int empty=0;
	uint32_t ret;
	//lock me 
	ttas_lock(&buffer.deq_lock);
	uint32_t diff = buffer.head - buffer.tail;
	if (diff>0){
		ret = buffer.array[buffer.tail & buffer.mask].value;
		*data=ret;
		buffer.tail++;
	}
	else empty=1;
	//unlock me
	ttas_unlock(&buffer.deq_lock);
	return empty;
} 
