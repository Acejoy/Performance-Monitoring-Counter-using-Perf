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

struct perf_entry{
    struct perf_event_attr pea;
    long long count;
    int fd;
};

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
    int m=5;
    struct perf_entry pArr[5];

    // struct perf_event_attr peArr[6];

    // long long countArr[6];

    // int fdArr[6];

    char *chars, c;

    uint64_t n;
    if (argc > 1) {
        n = strtoll(argv[1], NULL, 0);
    } else {
        n = 10000;
    }

    chars = malloc(n * sizeof(char));

    for(int i=0; i<m; i++){
        memset(&pArr[i].pea, 0, sizeof(struct perf_event_attr));     
    }

   pArr[0].pea.type = PERF_TYPE_HW_CACHE;
   pArr[1].pea.type = PERF_TYPE_HW_CACHE;
   pArr[2].pea.type = PERF_TYPE_HW_CACHE;
   pArr[3].pea.type = PERF_TYPE_HW_CACHE;
   pArr[4].pea.type = PERF_TYPE_HW_CACHE;
   pArr[5].pea.type = PERF_TYPE_SOFTWARE;

    
    for(int i=0; i<m; i++){
        pArr[i].pea.size = sizeof(struct perf_event_attr);
    }

    pArr[0].pea.config = PERF_COUNT_HW_CACHE_L1D |
               PERF_COUNT_HW_CACHE_OP_READ << 8 |
               PERF_COUNT_HW_CACHE_RESULT_MISS << 16;
        
    pArr[1].pea.config = PERF_COUNT_HW_CACHE_LL |
               PERF_COUNT_HW_CACHE_OP_READ << 8 |
               PERF_COUNT_HW_CACHE_RESULT_MISS << 16;
    
    pArr[2].pea.config = PERF_COUNT_HW_CACHE_LL |
                PERF_COUNT_HW_CACHE_OP_WRITE << 8 |
                PERF_COUNT_HW_CACHE_RESULT_MISS << 16;

    pArr[3].pea.config = PERF_COUNT_HW_CACHE_DTLB |
               PERF_COUNT_HW_CACHE_OP_READ << 8 |
               PERF_COUNT_HW_CACHE_RESULT_MISS << 16;
    
    pArr[4].pea.config = PERF_COUNT_SW_PAGE_FAULTS ;



    for(int i=0; i<m; i++){
        pArr[i].pea.disabled = 1;
        pArr[i].pea.exclude_kernel = 1;
        pArr[i].pea.exclude_hv = 1;

        pArr[i].fd = perf_event_open(&pArr[i].pea, 0, -1, -1, 0);

        if (pArr[i].fd == -1) {
            fprintf(stderr, "%d, Error opening leader %llx\n", i, pArr[i].pea.config);
            exit(EXIT_FAILURE);
        }
    }
  
    /* Write the memory to ensure misses later. */
    for (size_t i = 0; i < n; i++) {
        chars[i] = 1;
    }

    for(int i=0; i<m; i++){
        ioctl(pArr[i].fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(pArr[i].fd, PERF_EVENT_IOC_ENABLE, 0);
    }

    /* Read from memory. */
    for (size_t i = 0; i < n; i++) {
        c = chars[i];
    }

    for (size_t i = 0; i < n; i++) {
        chars[i] = (chars[i]+1)%256;
    }

    for(int i=0; i<m; i++){
        ioctl(pArr[i].fd, PERF_EVENT_IOC_DISABLE, 0);
        read(pArr[i].fd, &pArr[i].count, sizeof(long long));    
        close(pArr[i].fd);
    }

    printf("L1 Read misses: %lld\n", pArr[0].count);
    printf("LL Read misses: %lld\n", pArr[1].count);
    printf("LL Write misses: %lld\n", pArr[2].count);
    printf("TLB misses: %lld\n", pArr[3].count);
    printf("Page Faults: %lld\n", pArr[4].count);
    

    free(chars);
    exit(0);
}
