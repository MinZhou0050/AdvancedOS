// Name: Min Zhou
// Date: 3/20/2022
// Referenced code from Professor.Qin

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <sys/types.h>
#include <stdbool.h>

int buffer_n;          
int buffer_c;          
int buffer_p;          
int progarm_wait;          
int next_point;      
bool err_flag;  
int batch_job;

pthread_mutex_t count_mutex;  
pthread_cond_t buffer_not_full; 
pthread_cond_t buffer_not_empty; 

enum py{fcfs,sjf,priority,} policy;

typedef struct
{
    int cpu_time;                   
    int cpu_time_remaining;         
    int cpu_first_time;             
    int priority;                   
    int response_time;              
    int waiting_time;               
    int turnaround_time;            
    int finish_time;                
    time_t arrival_time;            
    char program[1000];  
    char job_name[1000]; 
} n_process;

typedef n_process *new_process;
typedef n_process *finished_process;

new_process running_processes[100];
new_process finished_processes[9999];
new_process current_process;

void process(new_process p);
void *dispatch(void *point);

void scheduler(int argc, char **argv);
new_process init_process(char **argv);
void sort_list(new_process *proc_list);
int switch_policy(const void *a, const void *b);
char *get_policy();
int set_policy(char *item);

int get_wait_time();
void performance(int job_num, int is_test);
void benchmark_ex(char *bench_name, int njobs, int arrival, int pri, int min_cpu, int max_cpu);

void err_msg(char *item, bool err_flag);

