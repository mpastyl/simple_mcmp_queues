#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<sys/time.h>
#include<unistd.h>
#include<pthread.h>
#include<sched.h>
#include "mcmp_bounded_lock_based.h"
//benchmark configurables
uint32_t no_threads=16;
uint32_t size = 1024*16;
int bench_duration_sec = 20;
//

//globals
volatile int stop=0;
volatile int barrier=0;
//

void setaffinity_oncpu(unsigned int cpu)
{
    cpu_set_t cpu_mask;
    int err;

    CPU_ZERO(&cpu_mask);
    CPU_SET(cpu, &cpu_mask);

    err = sched_setaffinity(0, sizeof(cpu_set_t), &cpu_mask);
    if (err) {
        perror("sched_setaffinity");
        exit(1);
    }
}
typedef struct thread_data_struct{
		int tid;
		struct timeval start;
		struct timeval end;
		int succ_enq;
		int succ_deq;
		uint32_t data_dump;
}thread_data_struct;

struct thread_data_struct * thread_data;


void thread_loop(void * tid_void_ptr){



	int tid = (int) tid_void_ptr;
	setaffinity_oncpu(tid%14);
	while(!barrier) {};
	struct thread_data_struct * my_data = & thread_data[tid];
	gettimeofday(&my_data->start, NULL);
	int succ_enq=0;
	int succ_deq=0;
	uint32_t data_dump=0;
	while (!stop){
		int ret;
		uint32_t r = rand();
		if (r %2){ //enqueue
			ret = enqueue(r);
			if (!ret) succ_enq++;
		}
		else{//dequeue
			uint32_t data =0;
			ret = dequeue(&data);
			if (!ret){
				data_dump |= data;
				succ_deq++;
			}
		}
	}
	gettimeofday(&my_data->end, NULL);
	my_data->succ_enq =  succ_enq;
	my_data->succ_deq =  succ_deq;
	my_data->data_dump = data_dump;
}

int main(){


	PRINTF(" calling init_queue\n");
    init_queue(size);
	
	
	int i;
	
	/*
	PRINTF("starting parrell for\n");
	
    uint32_t ret=0;
	#pragma omp parallel for num_threads(no_threads),reduction(+:ret)
	for(i=0; i<no_threads;i++){
		ret = enqueue(omp_get_thread_num());
	}
	PRINTF("All enqueues finished succesfull %d\n",no_threads-ret);
	#pragma omp parallel for num_threads(no_threads), private(ret)
	for(i=0; i<no_threads;i++){
		uint32_t data=0;
		ret = dequeue(&data);
		if(!ret) printf("dequeued %d\n",data);
	}
	*/

	//------  BENCHMARK -------
	
	srand(0);
	thread_data = (struct thread_data_struct *) malloc(no_threads * sizeof(thread_data_struct));
	for(i=0;i<no_threads;i++){
		thread_data->tid=0;
		thread_data->succ_enq=0;
		thread_data->succ_deq=0;
		thread_data->data_dump=0;
	}
	
	
	stop=0;
	barrier=0;
	pthread_t threads[no_threads];	
	for (i=0;i<no_threads;i++){
		thread_data->tid=i;
		int rc = pthread_create(&threads[i], NULL, thread_loop, (void *) thread_data->tid);
		if (rc){
			printf(" Could not launch all threads, exiting....\n");
			return 0;
		}
	}

	printf("Starting benchmark \n");
	barrier=1;
	sleep(bench_duration_sec);
	stop=1;
	/*
	#pragma omp parallel num_threads(no_threads+1) 
	{	
		// The extra thread sleeps and signals everyone to stop
		if (omp_get_thread_num()==no_threads){
			sleep(bench_duration_sec);
			stop=1;
		}
		else{
			thread_loop( omp_get_thread_num());
		}

	}
	*/
	printf("Benchmark finished\n");
	for(i=0; i<no_threads;i++){
		pthread_join(threads[i],NULL);
	}
	long int total_succ_ops = 0;
	int data_dump=0;
	for(i=0; i<no_threads;i++){
		total_succ_ops += thread_data[i].succ_enq;
		total_succ_ops += thread_data[i].succ_deq;
		data_dump|= thread_data[i].data_dump;
	}
	printf("data dump: %d \n",data_dump);
	double throughput = total_succ_ops / (float)bench_duration_sec;
	printf("THROUGHPUT: %f MOps per second \n",throughput/1e6);
    return 1;
}
