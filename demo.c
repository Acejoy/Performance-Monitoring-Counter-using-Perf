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


    // long long countArr[6];
    long long count;
    long long count2;

    // int fdArr[6];
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

    // for(int i=0; i<6; i++){
    //     memset(&peArr[i], 0, sizeof(struct perf_event_attr));     
    // }

    memset(&pe, 0, sizeof(struct perf_event_attr)); 
    memset(&pe2, 0, sizeof(struct perf_event_attr));

//    peArr[0].type = PERF_TYPE_HW_CACHE;
//    peArr[1].type = PERF_TYPE_HW_CACHE;
//    peArr[2].type = PERF_TYPE_HW_CACHE;
//    peArr[3].type = PERF_TYPE_HW_CACHE;
//    peArr[4].type = PERF_TYPE_HW_CACHE;
//    peArr[5].type = PERF_TYPE_SOFTWARE;

    
    pe.type = PERF_TYPE_HW_CACHE;
    pe2.type = PERF_TYPE_HW_CACHE;

    // pe.type = PERF_TYPE_HW_CACHE;
    
    // for(int i=0; i<6; i++){
    //     peArr[i].size = sizeof(struct perf_event_attr);
    // }

   pe.size = sizeof(struct perf_event_attr);
   pe2.size = sizeof(struct perf_event_attr);

    // peArr[0].config = PERF_COUNT_HW_CACHE_L1D |
    //            PERF_COUNT_HW_CACHE_OP_READ << 8 |
    //         //    PERF_COUNT_HW_CACHE_OP_WRITE << 8 |
    //            PERF_COUNT_HW_CACHE_RESULT_MISS << 16;
    
    // peArr[1].config = PERF_COUNT_HW_CACHE_L1D |
    //             PERF_COUNT_HW_CACHE_OP_WRITE << 8 |
    //             PERF_COUNT_HW_CACHE_RESULT_MISS << 16;

    // peArr[2].config = PERF_COUNT_HW_CACHE_LL |
    //            PERF_COUNT_HW_CACHE_OP_READ << 8 |
    //         //    PERF_COUNT_HW_CACHE_OP_WRITE << 8 |
    //            PERF_COUNT_HW_CACHE_RESULT_MISS << 16;
    
    // peArr[3].config = PERF_COUNT_HW_CACHE_LL |
    //             PERF_COUNT_HW_CACHE_OP_WRITE << 8 |
    //             PERF_COUNT_HW_CACHE_RESULT_MISS << 16;

    // peArr[4].config = PERF_COUNT_HW_CACHE_DTLB |
    //            PERF_COUNT_HW_CACHE_OP_READ << 8 |
    //         //    PERF_COUNT_HW_CACHE_OP_WRITE << 8 |
    //            PERF_COUNT_HW_CACHE_RESULT_MISS << 16;
    
    // peArr[5].config = PERF_COUNT_SW_PAGE_FAULTS ;


   pe.config = PERF_COUNT_HW_CACHE_L1D |
               PERF_COUNT_HW_CACHE_OP_READ   << 8 |
               PERF_COUNT_HW_CACHE_RESULT_MISS << 16;
    
    pe2.config = PERF_COUNT_HW_CACHE_LL |
                PERF_COUNT_HW_CACHE_OP_WRITE << 8 |
                PERF_COUNT_HW_CACHE_RESULT_MISS << 16;

    //   pe.config = PERF_COUNT_HW_CACHE_RESULT_MISS<<16 ;

    // for(int i=0; i<6; i++){
    //     peArr[i].disabled = 1;
    //     peArr[i].exclude_kernel = 1;
    //     peArr[i].exclude_hv = 1;

    //     fdArr[i] = perf_event_open(&peArr[i], 0, -1, -1, 0);

    //     if (fdArr[i] == -1) {
    //         fprintf(stderr, "Error opening leader %llx\n", peArr[i].config);
    //         exit(EXIT_FAILURE);
    //     }

    //     ioctl(fdArr[i], PERF_EVENT_IOC_RESET, 0);
    //     ioctl(fdArr[i], PERF_EVENT_IOC_ENABLE, 0);
    // }
  
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

    // for(int i=0; i<6; i++){
    //     ioctl(fdArr[i], PERF_EVENT_IOC_DISABLE, 0);
    //     read(fdArr[i], &count[i], sizeof(long long));    
    //     close(fdArr[i]);
    // }

    printf("L1 misses: %lld\n", count);
    printf("LL misses: %lld\n", count2);

    close(fd);
    close(fd2);
    free(chars);
    exit(0);
}
