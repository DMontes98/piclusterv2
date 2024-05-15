/****************************************************************************
 *
 *   Copyright (c) 2024 Daniel Montes <daniel.montestolon@slu.edu>
 *   AirCRAFT Lab - Saint Louis University
 *
 ****************************************************************************/

/**
 * @file MSCS.cpp
 * @author Daniel Montes 	<daniel.montestolon@slu.edu>
 * @date 06 Apr 2024
 * @brief Quick test of Real-Time POSIX thread
 * @see https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/application_base
 * @see https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/cyclic
 */

/** INCLUDES **/
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
#define RT_THREAD_RUNS                      100U
#define RT_THREAD_PERIOD_MS                 30U

/** TYPEDEFS **/
typedef struct rt_thread_period_info_t {
    struct timespec next_period;
    long unsigned period_ns;   
} rt_thread_period_info_t;

/** FUNCTION DEFINITION **/
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
void rt_thread_init(rt_thread_period_info_t* period_info) {
    /** FUNCTION BODY **/
    period_info->period_ns = RT_THREAD_PERIOD_MS*1e6;                                       
    /* Get Initial Timestamp */
    clock_gettime(CLOCK_MONOTONIC, &(period_info->next_period));  
}
void do_rt_task() {
    /* VARIABLE DEFINITION */
    static struct timespec base_timestamp;
    static struct timespec current_timestamp;
    static bool first_run = true;
    static long unsigned timestamp_ns;
    static int iteration_number = 0;
    
    /* FUNCTION BODY */
    /* Get current timestamp */
    clock_gettime(CLOCK_MONOTONIC, &current_timestamp);

    /* Check if it is the first time the task runs */
    if (first_run) {
        /* Get base Timestamp */
        clock_gettime(CLOCK_MONOTONIC, &base_timestamp);
        /* Print Welcome Message */
        printf("[RT THREAD] - Running RT Task with a period of %ums\n", RT_THREAD_PERIOD_MS); 
        first_run = false;
    }

    /* Compute Relative Timestamp */
    timestamp_ns = 1e9*(current_timestamp.tv_sec-base_timestamp.tv_sec)+(current_timestamp.tv_nsec-base_timestamp.tv_nsec);

    /* Print current iteration */
    printf("              [%luns] Running RT Task. Iteration Number %i\n", timestamp_ns, iteration_number++);
}

/** RT THREAD ENTRY POINT **/
void *rt_thread_entry() {
    /** VARIABLE DEFINITIONS **/
    rt_thread_period_info_t period_info;
    int run_count = 0;

    /** FUNCTION BODY **/
    /* RT Thread Initialization */
    rt_thread_init(&period_info);

    /* Main Loop */
    while (1) {
        /* Run RT Task */
        do_rt_task();
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

/** MAIN FUNCTION **/
int main(int argc, char* argv[]) {
    /** VARIABLE DEFINITION **/
    /* Capabilities Interface */
    cap_t caps;
    const cap_value_t cap_list[1] = {CAP_SYS_NICE};
    /* POSIX Threads */
    pthread_t rt_thread;
    pthread_attr_t rt_thread_attr;
    struct sched_param rt_sched_param;
    /* Return Values */
    int ret = 0; 

    /** FUNCTION BODY **/
    /** STEP 1: Lock Memory's Virtual Address Space into RAM **/
    /* Lock all current and future pages to prevent them from being swapped */
    ret = mlockall( MCL_CURRENT | MCL_FUTURE );
    if (ret) { 
        /* Error Handling */
        perror("[main()] - mlockall() failed\n");
        goto exit;       
    }

    /** STEP 2: Define RT Thread Attributes **/
    /* Initialize Attributes to Default Values */
    (void) pthread_attr_init(&rt_thread_attr);                                  // This function always succeeds on Linux

    /* Set RT Thread Stack Size */
    ret = pthread_attr_setstacksize(&rt_thread_attr, PTHREAD_STACK_MIN + RT_THREAD_STACK_SIZE);
    if (ret) {
        /* Error Handling */
        perror("[main()] - pthread_attr_setstacksize() failed\n");
        goto exit;   

    }

    /* Set Scheduling Policy to FIFO */
    ret = pthread_attr_setschedpolicy(&rt_thread_attr, SCHED_FIFO);
    if (ret) {
        perror("[main()] - pthread_attr_setschedpolicy() failed\n");
        goto exit;
    }

    /* Set Scheduling Priority */
    rt_sched_param.sched_priority = 1;
    ret = pthread_attr_setschedparam(&rt_thread_attr, &rt_sched_param);
    if (ret) {
        perror("[main()] - pthread_attr_setschedparam() failed\n");
        goto exit;
    }

    /* Use Scheduling Parameters defined in rt_thread_attr */
    ret = pthread_attr_setinheritsched(&rt_thread_attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
        perror("[main()] - pthread_attr_setinheritsched() failed\n");
        goto exit;
    }

    /** STEP 4: Create an POSIX RT thread with specified attributes **/
    ret = pthread_create(&rt_thread, &rt_thread_attr, rt_thread_entry, NULL);
    if (ret) {
        printf("[main()] - pthread_create() failed\n");
        goto exit;
    }

    /** STEP 5: Join the RT thread and wait until it is done **/   
    ret = pthread_join(rt_thread, NULL);
    if (ret) {
        printf("[main()] - pthread_join() failed\n");
        goto exit;
    }

exit:
    return ret;
}