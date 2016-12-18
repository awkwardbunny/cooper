#define SCHED_NPROC   1024
#define SCHED_READY    001
#define SCHED_RUNNING  002
#define SCHED_SLEEPING 003
#define SCHED_ZOMBIE   004 
#define STACK_SIZE   65536
#define IT_USEC     100000

#define E_NOERR     0
#define E_NOPID     1
#define E_SIGMASK   2
#define E_MMAP      3
#define E_SIGUMASK  4

#define DEF_NICE 0
#define DEF_TSLICE 10
#define SCHED_IN 5250
#define _return restorectx(&exited,0)

struct sched_proc {
    unsigned int task_state;
    unsigned int ticks;
    unsigned int timeslice;
    int nice;
    unsigned int priority;
    unsigned int pid;
    unsigned int ppid;
    void *sp_begin;
    struct savectx ctx;
    int exit_code;

    struct sched_proc *parent;
    struct sched_node *children;
    struct sched_node *mynode;
};

struct sched_node {
    struct sched_node *prev, *next;
    struct sched_proc *process;
};

struct sched_waitq {};

struct sched_node *run_anchor;
struct sched_proc *current;
unsigned short int pid_table[SCHED_NPROC + 1];

int errnom = 0;
extern void adjstack(void *, void *, unsigned long); 

int sched_init(void (*init_fn)());
unsigned int sched_fork();
void sched_exit(int code);
int sched_wait(int *exit_code);
void sched_nice(int niceval);
unsigned int sched_getpid();
unsigned int sched_getppid();
unsigned int sched_gettick();
void sched_ps(int);
int sched_switch();
void sched_tick(int);
unsigned int sched_newpid();
