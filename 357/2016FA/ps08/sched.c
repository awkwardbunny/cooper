#include <sys/time.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "jmpbuf-offsets64.h"
#include "savectx64.h"
#include "sched.h"

struct savectx exited;

void print_test(){
    int a;
    printf("%x\n", &a);
    for(a = 0; a < 2; a++)
        printf("Hello world!\n");

    restorectx(&exited, 0);
}

int main(int argc, char **argv){
    sched_init(print_test);
}

int sched_init(void (*init_fn)()){

    // Set Interval Timer
    struct itimerval itv_new;
    itv_new.it_interval.tv_sec = 0;
    itv_new.it_interval.tv_usec = IT_USEC;
    itv_new.it_value.tv_sec = 0;
    itv_new.it_value.tv_usec = IT_USEC;

    if(setitimer(ITIMER_VIRTUAL, &itv_new, NULL) < 0){
        fprintf(stderr, "Error: setitimer() failed: %s\n", strerror(errno));
        exit(-1);
    }

    // Set Timer Handler to sched_tick
    if(signal(SIGVTALRM, sched_tick) == SIG_ERR){
        fprintf(stderr, "Error: signal() setting SIGVTALRM handler failed: %s\n", strerror(errno));
        exit(-1);
    }

    // Initialize the pid_table
    memset(pid_table, 0, sizeof pid_table);
    pid_table[0] = 1; // 0 is not a valid free pid

    // Create a new stack
    void *sp_new;
    if((sp_new = mmap(0, STACK_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0)) == MAP_FAILED){
        fprintf(stderr, "Error: mmap() failed creating new stack for PID 1: %s\n", strerror(errno));
        exit(-1);
    }

    // Create and initialize a sched_proc (task_struct)
    struct sched_proc p_init;
    p_init.task_state = SCHED_RUNNING;
    p_init.ticks = 0;
    p_init.priority = 0;
    p_init.ppid = 1;
    p_init.pid = 1; // could've also used sched_newpid()
    pid_table[1] = 1;
    p_init.sp_begin = sp_new;

    // Create a new context and modify for the new process
    struct savectx new;
    savectx(&new);
    new.regs[JB_BP] = sp_new+STACK_SIZE-8;
    new.regs[JB_SP] = sp_new+STACK_SIZE-8;
    new.regs[JB_PC] = init_fn;

    *((int *)sp_new + 4) = 1;

    printf("%x:%x\n", sp_new, sp_new+STACK_SIZE);

    // Set the current pointer
    current = &p_init;

    // Save context to 'return' to label init_fn_returned
    // Kinda like using longjmp
    savectx(&exited);
    exited.regs[JB_PC] = &&init_fn_returned; // GCC specific: && gets address of label

    printf("ASHD\n");

    // Pass execution to init_fn
    restorectx(&new, 0);

init_fn_returned:
    fprintf(stderr, "Error: init_fn() returned! PANICCCCCC\n");
    exit(-1);
}

unsigned int sched_newpid(){
    int i = 0;
    for(; pid_table[i] && (i <= SCHED_NPROC); i++);
    if(i > SCHED_NPROC) return 0; 
    pid_table[i] = 1;
    return i;
}

int sched_fork(){
    /* create a new simulated task which is a copy of the caller.
    Allocated a new pid for the child, and make runnable and eligible
    to be scheduled. it returns 0 to child and stuff. not required that
    relative order of parent vs child be defined. -1 on error
    Do not need to duplicate entire address space. parent & child
    will execute in the same space. however, need to create a new
    private stack area and init to be a copy of the parent's */
}

int sched_exit(int code){
    /* terminate current task, making it a ZOMBIE, and store the exit code.
    if a parent is sleeping in sched_wait(), wake it up and return the exit
    code of it. there will be no equivalent of SIGCHILD. sched_exit will not
    return. another runnable process will be scheduled. */
}

int sched_wait(int *exit_code){
    /* return the exit code of a zombie child and free the resources of
    that child. if there is more tha one such child, the order in which
    the codes are returned is not defined. if none, but has at least one
    child, place caller in SLEEPING. to be woken up when a child calls
    sched_exit(). no children, return immediately with -1, otherwise
    return value is the pid of the child whose statuss is being returned.
    since no simulated signals, exit code is simply integer from sched_exit */
}

int sched_nice(int niceval){
    /* set current task's nice value to param. from +19 to -20. clamp */
}

unsigned int sched_getpid(){
    return current->pid;
}

unsigned int sched_getppid(){
    return current->ppid;
}

unsigned int sched_gettick(){
    return current->ticks;
}

int sched_ps(){
    /* output to stderr a listing of all of the current tasks, including
    sleeping and zombie tasks. list the following information in tabular form:
        pid
        ppid
        current state
        base addr of private stack area
        static priority
        dynamic priority info
        total CPU time used (in ticks)
    dynamic priority will vary in interpretation and range depending on what
    scheduling algorithm used. ex. CFS -> vruntime will be the best indicator. */
    /* this should be sighandler for SIGABRT so that a ps can be forced at any
    time by sending the testbed SIGABRT */
}

int sched_switch(){
    /* never be called directly by the testbed. the only place where a
    context switch is made, analogous to schedule(). should place current
    on the run queue then select the best READY and put in RUNNING state
    and context switch */
}

void sched_tick(int blah){
    /* sighandler for SIGVTALRM signal gen by the periodic timer
    examine currently running task and if its time slice just expired,
    mark that task as READY, place on run queue, and then call sched_switch()
    watch out for signal mask issues. remember SIGVTALRM will be by default
    masked on entry to sighandler */
}

