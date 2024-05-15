/****************************************************************************
 *
 *   Copyright (c) 2024 Daniel Montes <daniel.montestolon@slu.edu>
 *   AirCRAFT Lab - Saint Louis University
 *
 ****************************************************************************/

/**
 * @file MSCS.cpp
 * @author Daniel Montes 	<daniel.montestolon@slu.edu>
 * @date 07 Apr 2024
 * @brief Quick test of Real-Time POSIX thread with SLURM/MPI multithreading
 * @see https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/application_base
 * @see https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/cyclic
 */

/** INCLUDES **/
/* MPI */
#include <mpi.h>
/* Standard Libraries */
#include <errno.h>
#include <limits.h>
#include <pthread.h>                                                            // Must be linked using -lpthread
#include <stdbool.h>
#include <stdio.h>
#include <sys/capability.h>                                                     // Must be linked using -lcap
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>

/** DEFINES **/
#define RT_THREAD_STACK_SIZE                4096U
#define RT_THREAD_RUNS                      10U

/** TYPEDEFS **/
typedef struct rt_thread_period_info_t {
    struct timespec next_period;
    long unsigned period_ns;   
} rt_thread_period_info_t;

/** Global Variables **/
static struct timespec base_timestamp;
static unsigned rt_thread_period_ms[20] = {30, 40, 60, 20, 40, 55, 90, 50, 10, 10, 10, 20, 22, 46, 78, 55, 50, 20, 34, 40};

/** FUNCTION DEFINITION **/
int rt_init() {
    /** VARIABLE DEFINITION **/
    /* Capabilities Interface */
    cap_t caps;
    const cap_value_t cap_list[1] = {CAP_SYS_NICE};
    /* Return Value */
    int ret = 0;

    /** FUNCTION BODY **/
    /** STEP 1: Set proper permissions through the Capabilities interface **/
    /* Get Current Process Capabilities */
    caps = cap_get_proc();
    if (caps == NULL) {
        /* Error Handling */
        printf("[rt_init()] - cap_get_proc() failed: Could not get process capabilities for current process\n");
        ret = EPERM;
        goto exit;
    }

    /* Set Adequate Capability Flags */
    if (cap_set_flag(caps, CAP_EFFECTIVE, 1, cap_list, CAP_SET) == -1) {
        /* Error Handling */
        printf("[rt_init()] - cap_set_flag() failed: Could not set capability flags\n");
        ret = EPERM;
        goto exit;
    }
    
    /* Set new capability flags for the process */
    if (cap_set_proc(caps) == -1) {
        /* Error Handling */
        printf("[rt_init()] - cap_set_proc() failed: Could not set capability flags for current process\n");
        ret = EPERM;
        goto exit;
    }

    /* Free memory reserved for the caps_t variable*/
    if (cap_free(caps) == -1) {
        /* Error Handling */
        printf("[rt_init()] - cap_free() failed: Could not free memory used by capability flags\n");
        ret = EPERM;
        goto exit;       
    }

    /** STEP 2: Lock Memory's Virtual Address Space into RAM **/
    /* Lock all current and future pages to prevent them from being swapped */
    ret = mlockall( MCL_CURRENT | MCL_FUTURE );
    if (ret) { 
        /* Error Handling */
        perror("[rt_init()] - mlockall() failed\n");
        goto exit;       
    }
exit:
    return ret;
}
void rt_sleep(rt_thread_period_info_t* period_info) {
    /** FUNCTION BODY **/
    /** Increment Period **/
    period_info->next_period.tv_nsec += period_info->period_ns;
    
    /* Handle timespec nsec overflow */
    while (period_info->next_period.tv_nsec >= 1000000000) {
            period_info->next_period.tv_sec++;
            period_info->next_period.tv_nsec -= 1000000000;
    }

    /** Sleep until period has elapsed **/
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &period_info->next_period, NULL);
    /* NOTE: Signal wakes are not handled */
}
void rt_thread_init(rt_thread_period_info_t* period_info, int rank) {
    /** FUNCTION BODY **/
    period_info->period_ns = rt_thread_period_ms[rank]*1e6;                                       
    /* Get Initial Timestamp */
    clock_gettime(CLOCK_MONOTONIC, &(period_info->next_period));  
}
void do_rt_task(int rank) {
    /* VARIABLE DEFINITION */
    static struct timespec current_timestamp;
    static bool first_run = true;
    static long unsigned timestamp_ns;
    static int iteration_number = 0;
    /* Processor Names */
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int result_len;
    
    /* FUNCTION BODY */
    /* Get current timestamp */
    clock_gettime(CLOCK_MONOTONIC, &current_timestamp);

    /** Get name of processor that is running the thread **/
    MPI_Get_processor_name(processor_name, &result_len);

    /* Check if it is the first time the task runs */
    if (first_run) {
        /* Print Welcome Message */
        printf("[%s:R%i] - Spawned RT Task %ums\n", processor_name, rank, rt_thread_period_ms[rank]); 
        first_run = false;
    }

    /* Compute Relative Timestamp */
    timestamp_ns = 1e9*(current_timestamp.tv_sec-base_timestamp.tv_sec)+(current_timestamp.tv_nsec-base_timestamp.tv_nsec);

    /* Print current iteration */
    printf("[%s:R%i-%luns] - Running RT Task with a period of %ums [%i]\n", processor_name, rank, timestamp_ns, rt_thread_period_ms[rank], iteration_number++);
}

/** RT THREAD ENTRY POINTS **/
void *rt_thread_entry() {
    /** VARIABLE DEFINITIONS **/
    rt_thread_period_info_t period_info;
    int run_count = 0;
    int rank;

    /** FUNCTION BODY **/
    /* Get Rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* RT Thread Initialization */
    rt_thread_init(&period_info, rank);

    /* Main Loop */
    while (1) {
        /* Run RT Task */
        do_rt_task(rank);
        run_count++;

        /* Check if RT thread should exit */
        if (run_count >= RT_THREAD_RUNS) {
            break;
        }

        /* Sleep until period has elapsed */
        rt_sleep(&period_info);
    }

    /* Return Null */
    return NULL;
}
int create_rt_thread(pthread_t* rt_thread) {
    /** VARIABLE DEFINITION **/
    /* POSIX Threads */
    pthread_attr_t rt_thread_attr;
    struct sched_param rt_sched_param;
    /* Return Value */
    int ret = 0;

    /** FUNCTION BODY **/
    /** Define RT Thread Attributes **/
    /* Get base Timestamp */
    clock_gettime(CLOCK_MONOTONIC, &base_timestamp);

    /* Initialize Attributes to Default Values */
    (void) pthread_attr_init(&rt_thread_attr);                                  // This function always succeeds on Linux

    /* Set RT Thread Stack Size */
    ret = pthread_attr_setstacksize(&rt_thread_attr, PTHREAD_STACK_MIN + RT_THREAD_STACK_SIZE);
    if (ret) {
        /* Error Handling */
        perror("[create_rt_thread()] - pthread_attr_setstacksize() failed\n");
        goto exit;   

    }

    /* Set Scheduling Policy to FIFO */
    ret = pthread_attr_setschedpolicy(&rt_thread_attr, SCHED_FIFO);
    if (ret) {
        perror("[create_rt_thread()] - pthread_attr_setschedpolicy() failed\n");
        goto exit;
    }

    /* Set Scheduling Priority */
    rt_sched_param.sched_priority = 1;
    ret = pthread_attr_setschedparam(&rt_thread_attr, &rt_sched_param);
    if (ret) {
        perror("[create_rt_thread()] - pthread_attr_setschedparam() failed\n");
        goto exit;
    }

    /* Use Scheduling Parameters defined in rt_thread_attr */
    ret = pthread_attr_setinheritsched(&rt_thread_attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
        perror("[create_rt_thread()] - pthread_attr_setinheritsched() failed\n");
        goto exit;
    }

    /** Create an POSIX RT thread with specified attributes **/
    ret = pthread_create(rt_thread, &rt_thread_attr, rt_thread_entry, NULL);
    if (ret) {
        printf("[create_rt_thread()] - pthread_create() failed\n");
        goto exit;
    }
exit:
    return ret;
}

/** MAIN FUNCTION **/
int main(int argc, char* argv[]) {
    /** VARIABLE DEFINITION **/
    /* POSIX Threads */
    pthread_t rt_thread;
    /* Return Values */
    int ret = 0; 

    /** FUNCTION BODY **/
    /** Initialize RT Environment **/
    ret = rt_init();
    if (ret) { 
        /* Error Handling */
        perror("[main()] - rt_init() failed\n");
        goto exit;       
    }

    /** Initialize MPI **/
    MPI_Init(&argc, &argv);

    /** Create RT Thread **/
    create_rt_thread(&rt_thread);
    if (ret) {
        printf("[main()] - create_rt_thread() failed\n");
        goto exit;
    }


    /** Join the RT thread and wait until it is done **/   
    ret = pthread_join(rt_thread, NULL);
    if (ret) {
        printf("[main()] - pthread_join() failed\n");
        goto exit;
    }

    /** Finalize MPI **/
    MPI_Finalize();
exit:
    return ret;
}