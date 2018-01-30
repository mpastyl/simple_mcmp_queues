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

static inline unsigned long* seed_rand() 
{
  unsigned long* seeds;
  /* seeds = (unsigned long*) ssalloc_aligned(64, 64); */
  //seeds = (unsigned long*) memalign(64, 64);
  seeds = (unsigned long*) calloc(3,64);
  seeds[0] = getticks() % 123456789;
  seeds[1] = getticks() % 362436069;
  seeds[2] = getticks() % 521288629;
  return seeds;
}

#define my_random xorshf96

static inline unsigned long
xorshf96(unsigned long* x, unsigned long* y, unsigned long* z)  //period 2^96-1
{
  unsigned long t;
  (*x) ^= (*x) << 16;
  (*x) ^= (*x) >> 5;
  (*x) ^= (*x) << 1;

  t = *x;
  (*x) = *y;
  (*y) = *z;
  (*z) = t ^ (*x) ^ (*y);

  return *z;
}

void busy_loop(uint64_t iters) {
    volatile int sink;
    do {
        sink = 0;
    } while (--iters > 0);
    (void)sink;
}

