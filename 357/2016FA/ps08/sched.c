#include <sys/time.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "jmpbuf-offsets64.h"
#include "savectx64.h"
#include "sched.h"

struct savectx exited; // Context to be used when init_fn 'returns'
sigset_t blockset; // Set of signals to block

void print_test(){
    printf("Hello world!\n");

    for(int i = 0; i < 5; i++){
        switch(sched_fork()){
            case -1:
                fprintf(stderr, "Fork failed wtf\n");
                fprintf(stderr, "Errnom is %d: %s", errnom, strerror(errno));
                _return;
            case 0:
                printf("Hello world from child %d\n", sched_getpid());
                if(sched_getpid() == 3) sched_nice(-1);
                if(sched_getpid() == 6) sched_nice(-4);
                if(sched_getpid() == 4){
                    if(sched_fork()){
                        sched_exit(101);
                    }
                }
                break;
            default:
                printf("Continue to fork from %d\n", sched_getpid());
                continue;
        }
        break;
    }

/*
    if(!sched_fork()){
        printf("ASD\n");
        sched_exit(3);
    }else{
        printf("1\n");
        for(int i = 0; i < 1000000000; i++){}
        printf("2\n");

        int cp = 0;
        //printf("sched_wait returned %d from pid %d\n", sched_wait(&cp), cp);
    }
*/

    while(1);

    _return; // MACRO defined in sched.h
}

int main(int argc, char **argv){
    sched_init(print_test);
    fprintf(stderr, "Error: sched_init() returned!!!\n");
    exit(-1);
} 

int sched_init(void (*init_fn)()){
    // Set the signal set structs
    if(sigfillset(&blockset) < 0){
        fprintf(stderr, "Error: could not set signals.\n");
        exit(-1);
    }

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
    p_init.nice = DEF_NICE;
    p_init.priority = DEF_NICE + 20;
    p_init.sp_begin = sp_new;
    p_init.exit_code = 0;
    p_init.timeslice = DEF_TSLICE;
    p_init.ppid = 1;
    p_init.pid = 1; // could've also used sched_newpid()
    pid_table[1] = 1;

    // Create a new context and modify for the new process
    struct savectx new;
    savectx(&new);
    new.regs[JB_BP] = sp_new+STACK_SIZE;
    new.regs[JB_SP] = sp_new+STACK_SIZE;
    new.regs[JB_PC] = init_fn;

    // Set the current pointer
    current = &p_init;

    // Save context to 'return' to label init_fn_returned
    // Kinda like using longjmp
    savectx(&exited);
    exited.regs[JB_PC] = &&init_fn_returned; // GCC specific: && gets address of label

    // Initialize the running queue (and insert current process)
    struct sched_node anchor;
    anchor.process = NULL;
    run_anchor = &anchor;
//    anchor.prev = &anchor;
//    anchor.next = &anchor;
    // Skip above lines, go directly to
    struct sched_node pn_init;
    anchor.prev = &pn_init;
    anchor.next = &pn_init;
    pn_init.prev = &anchor;
    pn_init.next = &anchor;
    pn_init.process = &p_init;
    p_init.mynode = &pn_init;

    // Initialize children list
    struct sched_node pn_children;
    pn_children.process = NULL;
    pn_children.next = &pn_children;
    pn_children.prev = &pn_children;
    p_init.children = &pn_children;

    // Set Timer Handler to sched_tick
    if(signal(SIGVTALRM, sched_tick) == SIG_ERR){
        fprintf(stderr, "Error: signal() setting SIGVTALRM handler failed: %s\n", strerror(errno));
        exit(-1);
    }

    // Set Abort Handler to sched_ps
    if(signal(SIGABRT, sched_ps) == SIG_ERR){
        fprintf(stderr, "Error: signal() setting SIGABRT handler failed: %s\n", strerror(errno));
        exit(-1);
    }

    // Pass execution to init_fn
    restorectx(&new, 0);

init_fn_returned:
    fprintf(stderr, "Error: init_fn() returned! PANICCCCCC\n");
    return -1;
}

unsigned int sched_newpid(){
    int i = 0;
    for(; pid_table[i] && (i <= SCHED_NPROC); i++);
    if(i > SCHED_NPROC) return 0; 
    pid_table[i] = 1;
    return i;
}

unsigned int sched_fork(){
    sigset_t origset; // Original set of signals that were blocked

    // Block all signals
    if(sigprocmask(SIG_BLOCK, &blockset, &origset) < 0){
        errnom = E_SIGMASK;
        return -1;
    }

    // Create and copy a new sched_proc
    struct sched_proc *p_child = malloc(sizeof(struct sched_proc));
    memcpy(p_child, current, sizeof(struct sched_proc));
    p_child->ppid = current->pid;
    p_child->parent = current;
    p_child->task_state = SCHED_READY;
    p_child->pid = sched_newpid();
    if(!p_child->pid){
        free(p_child);
        errnom = E_NOPID;
        return -1;
    }

    // Create a new stack area
    void *sp_new;
    if((sp_new = mmap(0, STACK_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0)) == MAP_FAILED){
        errnom = E_MMAP;
        return -1;;
    }

    // Save context; if scheduled in, then it is currently in child, return 0
    if(savectx(&(p_child->ctx)) == SCHED_IN){
        sigprocmask(SIG_SETMASK, &origset, NULL);
        return 0;
    }

    // Copy the parent's stack over
    memcpy(sp_new, current->sp_begin, STACK_SIZE);

    // Fix new stack
    unsigned long offset = sp_new - current->sp_begin;
    p_child->sp_begin = sp_new;
    p_child->ctx.regs[JB_SP] += offset;
    p_child->ctx.regs[JB_BP] += offset;
    adjstack(p_child->ctx.regs[JB_SP], (p_child->sp_begin)+STACK_SIZE, offset);

    // Add to run queue
    struct sched_node *pn_child = malloc(sizeof(struct sched_node));
    pn_child->process = p_child;
    p_child->mynode = pn_child;

    // Below adds to end of queue
    pn_child->next = run_anchor;
    pn_child->prev = run_anchor->prev;
    run_anchor->prev = pn_child;
    pn_child->prev->next = pn_child;

    // Below adds to right after current
//    pn_child->next = current->mynode->next;
//    pn_child->prev = current->mynode;
//    current->mynode->next = pn_child;
//    pn_child->next->prev = pn_child;

    // Initialize children list
    struct sched_node *pn_children = malloc(sizeof(struct sched_node));
    p_child->children = pn_children;
    pn_children->process = NULL;
    pn_children->prev = pn_children;
    pn_children->next = pn_children;

    // Add to children list of the parent(current)
    struct sched_node *pn_child2 = malloc(sizeof(struct sched_node));
    pn_child2->process = p_child;
    pn_child2->next = current->children->next;
    pn_child2->prev = current->children;
    current->children->next = pn_child2;
    pn_child2->next->prev = pn_child2;

    // Set original sigset back
    if(sigprocmask(SIG_SETMASK, &origset, NULL) < 0){
        errnom = E_SIGUMASK;
        free(pn_child);
        free(pn_child2);
        free(pn_children);
        // Well fuck it, if it can't unblock, its a fatal error and
        // wherever fork returns to should error check and terminate;
        return -1;
    };

    return p_child->pid;
}

void sched_exit(int code){
    // Should I let this return?
    // Problem set states that it shouldn't exit/return, so doesn't matter
    if(current->pid == 1){
        _return;
    }

    // Original set of signals that were blocked
    sigset_t origset;

    // Block all signals
    sigprocmask(SIG_BLOCK, &blockset, &origset);

    // Set its values
    current->task_state = SCHED_ZOMBIE;
    current->exit_code = code;
    pid_table[current->pid] = 0;

    printf("In exit\n");

    // Re-parent children, if any
    if(current->children->next->process){
        printf("Shouldnt be here\n");
        struct sched_node *node = current->children;
        for(node = node->next; node->process; node = node->next){
            node->process->ppid = current->ppid;
            node->process->parent = current->parent;
        }

        // Link children to parent
        current->children->prev->next = current->parent->children;
        current->children->next->prev = current->parent->children->prev;
        current->parent->children->prev->next = current->children->next;
        current->parent->children->prev = current->children->prev;
    }

    if(current->parent->task_state == SCHED_SLEEPING){
        current->parent->task_state = SCHED_READY;
    }

    sched_switch(); // This should not return
    fprintf(stderr, "Error: sched_exit() returned????\n");
    sigprocmask(SIG_SETMASK, &origset, NULL);
    _return;
}

int sched_wait(int *exit_code){

    // Original set of signals that were blocked
    sigset_t origset;

    // Block all signals
    sigprocmask(SIG_BLOCK, &blockset, &origset);
/*
    // If there are ZOMBIE children
    int zomb = 0;
    struct sched_node *node = current->children;
    for(node = node->next; node->process; node = node->next){
        if(node->process->task_state == SCHED_ZOMBIE){
            zomb = 1;
            break;
        }
    }

    if(!zomb){
        current->task_state = SCHED_SLEEPING;

    }
    
    *exit_code = node->process->exit_code;
    sigprocmask(SIG_SETMASK, &origset, NULL);
    return node->process->pid;
*/
    sigprocmask(SIG_SETMASK, &origset, NULL);
    return 0;

    /* return the exit code of a zombie child and free the resources of
    that child. if there is more tha one such child, the order in which
    the codes are returned is not defined. if none, but has at least one
    child, place caller in SLEEPING. to be woken up when a child calls
    sched_exit(). no children, return immediately with -1, otherwise
    return value is the pid of the child whose statuss is being returned.  since no simulated signals, exit code is simply integer from sched_exit */
}

void sched_nice(int niceval){
    if(niceval > 19)
        current->nice = 19;
    else if(niceval > -21)
        current->nice = niceval;
    else
        current->nice = -20;
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

void sched_ps(int blah){
    fprintf(stderr, "Init's children: ");
    struct sched_node *abc = run_anchor->next->process->children;
    for(abc = abc->next; abc->process; abc = abc->next){
        fprintf(stderr, "%d ", abc->process->pid);
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "PID\tPPID\tSTATE\tSTACK BASE\tNICE\tPRIOR\tTICKS\n");
    struct sched_node *node = run_anchor;
    for(node = node->next; node->process; node = node->next){
        fprintf(stderr, "%d\t%d\t", node->process->pid, node->process->ppid);
        
        switch(node->process->task_state){
            case SCHED_READY:
                fprintf(stderr, "READY\t");
                break;
            case SCHED_RUNNING:
                fprintf(stderr, "RUNNING\t");
                break;
            case SCHED_SLEEPING:
                fprintf(stderr, "SLEEPING\t");
                break;
            case SCHED_ZOMBIE:
                fprintf(stderr, "ZOMBIE\t");
        }

        fprintf(stderr, "%p\t%d\t%d\t%d\n", node->process->sp_begin, node->process->nice, node->process->priority, node->process->ticks);
    }
}

int sched_switch(){
    sigset_t origset; // Original set of signals that were blocked

    // Block all signals
    sigprocmask(SIG_BLOCK, &blockset, &origset);

    // Pick next READY process

    /* Picks the process with the highest priority    *
     * that still has a non-zero timeslice left.      *
     * If none of processes have a nonzero timeslice, *
     * the_chosen_one remains NULL and loop once more *
     * after (or as) timeslices are reset             */
     
    unsigned int max_prior = 0;
    struct sched_proc *the_chosen_one = NULL;

    struct sched_node *node = run_anchor;
    for(node = node->next; node->process; node = node->next){
        // Update the priorities
        /* Priority is inverse of nice value with
                nice=19  --> prior=0 and
                nice=-20 --> prior=39                */
        node->process->priority = (-1)*node->process->nice+19;

        if(node->process->task_state == SCHED_READY && node->process->priority > max_prior && node->process->timeslice){
            max_prior = node->process->priority;
            the_chosen_one = node->process;
        }
    }

    // Case described above where no processes have timeslices left
    if(!the_chosen_one){
        for(node = node->next; node->process; node = node->next){
            node->process->timeslice = DEF_TSLICE; // MEH

            if(node->process->task_state == SCHED_READY && node->process->priority > max_prior){
                max_prior = node->process->priority;
                the_chosen_one = node->process;
            }
        }
    }

    // By here, the_chosen_one should not be NULL
    // Unless something went horribly wrong
    // Save context and then restore to new process
    if(savectx(&(current->ctx)) != SCHED_IN){
        the_chosen_one->task_state = SCHED_RUNNING;
        current = the_chosen_one;

        restorectx(&(the_chosen_one->ctx), SCHED_IN);
    }

    // Unblock signals
    sigprocmask(SIG_SETMASK, &origset, NULL);
}

void sched_tick(int blah){
    current->ticks++;
    if(--(current->timeslice) == 0){
        current->task_state = SCHED_READY;
        sched_switch();
    }
}

