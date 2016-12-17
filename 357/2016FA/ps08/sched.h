#define SCHED_NPROC   1024
#define SCHED_READY    001
#define SCHED_RUNNING  002
#define SCHED_SLEEPING 003
#define SCHED_ZOMBIE   004

#define STACK_SIZE    1024
#define IT_USEC     100000

struct sched_proc {
    unsigned int task_state;
    unsigned int ticks;
    int nice;
    unsigned int priority;
    unsigned int pid;
    unsigned int ppid;
    void *sp_begin;
    struct savectx ctx;
};

struct sched_waitq {
    /* use this for each event/wakeup queue */
};

struct sched_proc *current;
unsigned short int pid_table[SCHED_NPROC + 1];

int sched_init(void (*init_fn)());
int sched_fork();
int sched_exit(int code);
int sched_wait(int *exit_code);
int sched_nice(int niceval);
unsigned int sched_getpid();
unsigned int sched_getppid();
unsigned int sched_gettick();
int sched_ps();
int sched_switch();
void sched_tick(int);
unsigned int sched_newpid();
