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

int N =256;
int block=1;

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




int main(int argc, char **argv)
{    
    int m=5;
    struct perf_entry pArr[5];


    // int N;
    // cin>>N;
    if(argc==3){
        N = atoi(argv[1]);
        block = atoi(argv[2]);
    } else if (argc==2) {
        N = atoi(argv[1]);
    } else {
        N = 256;
        block = 1;
    }

    // printf("%d %d\n",N, block);


    int (*A)[N] = malloc(sizeof(int[N][N]));
	int (*B)[N] = malloc(sizeof(int[N][N]));
    int (*C)[N] = malloc(sizeof(int[N][N]));

    

    /* Write the memory to ensure misses later. */    
    
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            B[i][j]=rand()/1000;
            C[i][j]=rand()/1000;
            A[i][j]=0;
        }
    }
    
    
    for(int i=0; i<m; i++){
        memset(&pArr[i].pea, 0, sizeof(struct perf_event_attr));     
    }

   pArr[0].pea.type = PERF_TYPE_HW_CACHE;
   pArr[1].pea.type = PERF_TYPE_HW_CACHE;
   pArr[2].pea.type = PERF_TYPE_HW_CACHE;
   pArr[3].pea.type = PERF_TYPE_HW_CACHE;
   pArr[4].pea.type = PERF_TYPE_SOFTWARE;

    
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
  
    

    for(int i=0; i<m; i++){
        ioctl(pArr[i].fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(pArr[i].fd, PERF_EVENT_IOC_ENABLE, 0);
    }

    /* Read from memory. */
    

    for(int i=0; i<N; i=i+block){
        for(int k=0; k<N; k=k+block){
            for (int j=0; j < N; j=j+block)
                for(int ii=i; ii<i+block; ii++)
                    for(int jj=j; jj < j+block; jj++)
                        for(int kk=k; kk < k+block; kk++)
                            A[ii][jj] += B[ii][kk] * C[kk][jj];
        }
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
    
    exit(0);
}