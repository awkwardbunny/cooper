struct sched_proc {
    int task_state;
    int ticks;
    long long *stack_addr;
    unsigned int priority;
    int pid;
    int ppid;

    /* use this for each simulated task */
    /* probably include task state, etc */
    /* proiority, accumulated, cpu time, stack address, etc */
};

struct sched_waitq {
    /* use this for each event/wakeup queue */
};

#define SCHED_NPROC511
#define SCHED_READY    001
#define SCHED_RUNNING  002
#define SCHED_SLEEPING 003
#define SCHED_ZOMBIE   004

sched_init(void (*init_fn)());
sched_fork();
sched_exit(int code);
sched_wait(int *exit_code);
sched_nice(int niceval);
sched_getpid();
sched_getppid();
sched_gettick();
sched_ps();
sched_switch();
sched_tick();

