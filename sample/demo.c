#define _GNU_SOURCE
#include <asm/unistd.h>
#include <linux/perf_event.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <inttypes.h>

static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                int cpu, int group_fd, unsigned long flags)
{
    int ret;

    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                    group_fd, flags);
    return ret;
}




int
main(int argc, char **argv)
{
    struct perf_event_attr pe;
    struct perf_event_attr pe2;


    // struct perf_event_attr peArr[6];

    
    long long count;
    long long count2;

    int fd;
    int fd2;
    char *chars, c;

    uint64_t n;
    if (argc > 1) {
        n = strtoll(argv[1], NULL, 0);
    } else {
        n = 10000;
    }

    chars = malloc(n * sizeof(char));

    memset(&pe, 0, sizeof(struct perf_event_attr)); 
    memset(&pe2, 0, sizeof(struct perf_event_attr));

   pe.type = PERF_TYPE_HW_CACHE;
   pe2.type = PERF_TYPE_HW_CACHE;

    // pe.type = PERF_TYPE_HW_CACHE;
    
   pe.size = sizeof(struct perf_event_attr);
   pe2.size = sizeof(struct perf_event_attr);

   pe.config = PERF_COUNT_HW_CACHE_L1D |
               PERF_COUNT_HW_CACHE_OP_READ << 8 |
            //    PERF_COUNT_HW_CACHE_OP_WRITE << 8 |
               PERF_COUNT_HW_CACHE_RESULT_MISS << 16;
    
    pe2.config = PERF_COUNT_HW_CACHE_LL |
                PERF_COUNT_HW_CACHE_OP_READ << 8 |
                PERF_COUNT_HW_CACHE_RESULT_MISS << 16;

    //   pe.config = PERF_COUNT_HW_CACHE_RESULT_MISS<<16 ;
  
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    // Don't count hypervisor events.
    pe.exclude_hv = 1;

    pe2.disabled = 1;
    pe2.exclude_kernel = 1;
    // Don't count hypervisor events.
    pe2.exclude_hv = 1;

    fd = perf_event_open(&pe, 0, -1, -1, 0);
    fd2 = perf_event_open(&pe2, 0, -1, -1, 0);
    
    if (fd == -1) {
        fprintf(stderr, "Error opening leader %llx\n", pe.config);
        exit(EXIT_FAILURE);
    }

    if (fd2 == -1) {
        fprintf(stderr, "Error opening leader %llx\n", pe2.config);
        exit(EXIT_FAILURE);
    }

    /* Write the memory to ensure misses later. */
    for (size_t i = 0; i < n; i++) {
        chars[i] = 1;
    }

    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    ioctl(fd2, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd2, PERF_EVENT_IOC_ENABLE, 0);

    /* Read from memory. */
    for (size_t i = 0; i < n; i++) {
        c = chars[i];
    }

    for (size_t i = 0; i < n; i++) {
        chars[i] = (chars[i]+1)%256;
    }

    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    read(fd, &count, sizeof(long long));

    ioctl(fd2, PERF_EVENT_IOC_DISABLE, 0);
    read(fd2, &count2, sizeof(long long));

    printf("L1 misses: %lld\n", count);
    printf("LL misses: %lld\n", count2);

    close(fd);
    close(fd2);
    free(chars);
    exit(0);
}
