#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
//#include "include/atomic_ops_if.h"
//#include "include/lock_if.h"

uint32_t * buffer;
uint32_t head;
uint32_t tail;
uint32_t mask;
uint32_t N;
//TODO: padd me

void init_queue(int size){
	
	N = size;
	head = 0;
	tail = 0;
	mask = N-1;
	assert((N & mask)==0 && "Size of buffer was not a power of 2");
	buffer = (uint32_t *) calloc(N, sizeof(uint32_t));
	
}

void enqueue(uint32_t data){
		
	//lock me 
	while(1){
		uint32_t diff  = head - tail;
		if (diff<N){
			buffer[head & mask] = data;
			head++;
			break;	
		}
	}
	//unlock me
	
}

uint32_t dequeue(){

	uint32_t ret;
	//lock me 
	while(1){
		uint32_t diff = head -tail;
		if (diff>0){
			ret = buffer[tail & mask];
			tail++;
			break;
		}
	}
	//unlock me
	return ret;
}

int main(){
	
	N = 4;	

	uint32_t ret;	
	
	init_queue(N);
	
	enqueue(13);
	enqueue(14);
	enqueue(15);
	enqueue(16);
	ret = dequeue();
	printf(" I dequeued %d \n",ret);
	enqueue(17);
	ret = dequeue();
	printf(" I dequeued %d \n",ret);
	enqueue(18);

	return 1;
}
