#include "sched.h"

int main(int argc, char **argv){

}

sched_init(void (*init_fn)()){
    /* call once by testbed program; init scheduling system
    including setting up PIT (setitimer), est. sched_tick as
    the signal for that timer, create initial task with pid 1
    make it runnable and transfer execution to it (including
    switching to its stack at location init_fn. Init_fn is not
    expected to return and if it does so it is OK to have 
    unpredictable results. */
}

sched_fork(){
    /* create a new simulated task which is a copy of the caller.
    Allocated a new pid for the child, and make runnable and eligible
    to be scheduled. it returns 0 to child and stuff. not required that
    relative order of parent vs child be defined. -1 on error
    Do not need to duplicate entire address space. parent & child
    will execute in the same space. however, need to create a new
    private stack area and init to be a copy of the parent's */
}

sched_exit(int code){
    /* terminate current task, making it a ZOMBIE, and store the exit code.
    if a parent is sleeping in sched_wait(), wake it up and return the exit
    code of it. there will be no equivalent of SIGCHILD. sched_exit will not
    return. another runnable process will be scheduled. */
}

sched_wait(int *exit_code){
    /* return the exit code of a zombie child and free the resources of
    that child. if there is more tha one such child, the order in which
    the codes are returned is not defined. if none, but has at least one
    child, place caller in SLEEPING. to be woken up when a child calls
    sched_exit(). no children, return immediately with -1, otherwise
    return value is the pid of the child whose statuss is being returned.
    since no simulated signals, exit code is simply integer from sched_exit */
}

sched_nice(int niceval){
    /* set current task's nice value to param. from +19 to -20. clamp */
}

sched_getpid(){
}

sched_getppid(){
}

sched_gettick(){
}

sched_ps(){
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

sched_switch(){
    /* never be called directly by the testbed. the only place where a
    context switch is made, analogous to schedule(). should place current
    on the run queue then select the best READY and put in RUNNING state
    and context switch */
}

sched_tick(){
    /* sighandler for SIGVTALRM signal gen by the periodic timer
    examine currently running task and if its time slice just expired,
    mark that task as READY, place on run queue, and then call sched_switch()
    watch out for signal mask issues. remember SIGVTALRM will be by default
    masked on entry to sighandler */
}

