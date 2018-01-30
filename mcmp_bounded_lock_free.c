#include "mcmp_bounded_lock_free.h"

typedef struct node_t
{
	uint32_t value;
	uint32_t used_slot; //1 means there is data to read from that slot, 0 otherwise
	uint8_t pad[64-2*sizeof(uint32_t)];
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
	
	node_t * array;
}buf_t;

struct buf_t buffer;

void init_queue(uint32_t size){
	
	buffer.N = size;
	buffer.head = size/2; //populate half the queue
	buffer.tail = 0;
	buffer.mask = buffer.N-1;
	assert((buffer.N & buffer.mask)==0 && "Size of buffer was not a power of 2");
	buffer.array = (node_t *) calloc(buffer.N, sizeof(node_t));
	int i;
	for( i=buffer.tail; i<buffer.head; i++){
		buffer.array[i & buffer.mask].used_slot = 1; //populate the used slots
	}
}

void print_size(){

	printf("Number of elements in the buffer %d\n",buffer.head-buffer.tail);
	printf("head is %d tail is %d\n",buffer.head,buffer.tail);
}

int enqueue(uint32_t data){
		
	uint32_t cp_head = buffer.head;
	uint32_t diff  = cp_head - buffer.tail;
	while (diff<buffer.N){
		if( __sync_val_compare_and_swap( &(buffer.head), cp_head, cp_head+1) == cp_head){
			// I managed to get a slot. Now i have to put data and return success
			buffer.array[cp_head & buffer.mask].value = data;
			buffer.array[cp_head & buffer.mask].used_slot = 1;
			return 0;//0 means success
		}
		cp_head =  buffer.head;
		diff = cp_head - buffer.tail;
	}
	return 1;

}

int dequeue(uint32_t * data){

	uint32_t ret;
	uint32_t cp_tail = buffer.tail;
	uint32_t diff = buffer.head - cp_tail;
	while (diff>0){
		if( __sync_val_compare_and_swap( &(buffer.tail), cp_tail, cp_tail+1) == cp_tail){
			// I managed to get a slot. I might need to check in case the enqueuer didn't put data inside yet.
			while (!(buffer.array[cp_tail & buffer.mask].used_slot)){}
			ret = buffer.array[cp_tail & buffer.mask].value;
			*data=ret;
			buffer.array[cp_tail & buffer.mask].used_slot = 0;
			return 0;
		}
		cp_tail = buffer.tail;
		diff  = buffer.head - cp_tail;
	}
	return 1;
} 
