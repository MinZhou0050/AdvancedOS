// Name: Min Zhou
// Date: 3/20/2022
// Referenced code from Professor.Qin

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "schedule.h"

void *dispatch(void *point)
{
    while(1)
    {
        pthread_mutex_lock(&count_mutex);

        while(progarm_wait == 0)
        {
            pthread_cond_wait(&buffer_not_empty, &count_mutex);
        }
        current_process = running_processes[buffer_p];

        pthread_cond_signal(&buffer_not_full);
        pthread_mutex_unlock(&count_mutex);

        process(current_process);

        progarm_wait--; 
        buffer_p++;
        buffer_p %= 100;

        current_process = NULL;
    }
}


void process(new_process p)
{
    char *path = p->program;
    char data[99];
    int new_cpu = p->cpu_time_remaining;

    sprintf(data, "%d", new_cpu);
    char *args[] = {path, "0", NULL};

    if (!strcmp(p->program, "./soft"))
    {
        args[1] = data;
    }

    pid_t child_process, wpid;
    int child_status = 0;
    child_process = fork();

    if (child_process == -1)
    {
        printf("Fork() has faile.\n");
    }
    else if (child_process == 0)
    {
        execv(path, args);
        printf("Execv() must have fail");
        exit(0);
    }

    if (p->cpu_first_time == 0)
    {
        p->cpu_first_time = time(NULL);
    }

    while ((wpid = wait(&child_status)) > 0);

    memcpy(&finished_processes[next_point], &running_processes[buffer_p], sizeof(current_process));

    finished_processes[next_point]->finish_time = time(NULL);
    finished_processes[next_point]->turnaround_time = finished_processes[next_point]->finish_time - finished_processes[next_point]->arrival_time;
    if (finished_processes[next_point]->turnaround_time)
    {
        finished_processes[next_point]->waiting_time = finished_processes[next_point]->turnaround_time - finished_processes[next_point]->cpu_time;
    }
    else
    {
        finished_processes[next_point]->waiting_time = 0;
    }
    finished_processes[next_point]->response_time = finished_processes[next_point]->cpu_first_time - finished_processes[next_point]->arrival_time;

    next_point++;

    running_processes[buffer_p]->cpu_time_remaining = 0;
}


void scheduler(int argc, char **argv)
{
    pthread_mutex_lock(&count_mutex);

    while(progarm_wait == 100)
    {
        pthread_cond_wait(&buffer_not_full, &count_mutex);
    }

    pthread_mutex_unlock(&count_mutex);

    new_process new_p = init_process(argv);

    char *n_policy = get_policy();
    printf("Job %s was submitted.\n", new_p->job_name);
    printf("Total number of jobs in the queue: %d\n", progarm_wait+1);
    printf("Expected waiting time: %d seconds\n", get_wait_time());
    printf("Scheduling Policy: %s.\n", n_policy);

    running_processes[buffer_n] = new_p;

    pthread_mutex_lock(&count_mutex);

    progarm_wait++;
    buffer_n++;

    sort_list(running_processes);
    buffer_n %= 100;

    pthread_cond_signal(&buffer_not_empty);
    pthread_mutex_unlock(&count_mutex);
}


new_process init_process(char **argv)
{
    new_process p = malloc(sizeof(n_process));

    char new_name[99];
    int id = buffer_n;

    sprintf(new_name, "job%d", id);

    p->cpu_time = atoi(argv[2]);
    p->cpu_time_remaining = atoi(argv[2]);
    p->cpu_first_time = 0;
    p->priority = atoi(argv[3]);
    p->response_time = 0;
    p->waiting_time = 0;
    p->turnaround_time = 0;
    p->finish_time = 0;
    p->arrival_time = time(NULL);
    strcpy(p->job_name, new_name);
    strcpy(p->program, argv[1]);

    return p;
}


void sort_list(new_process *proc_list)
{
    int i;
    void *sort_algo = switch_policy;

    if (!batch_job)
    {
        i = buffer_p + 1;
    }
    else
    {
        i = buffer_p;
    }

    qsort(&proc_list[i], buffer_n-i, sizeof(new_process), sort_algo);
}


int switch_policy(const void *a, const void *b)
{
    new_process p_a = *(new_process *)a;
    new_process p_b = *(new_process *)b;

    if (policy == fcfs)
    {
        return (p_a->arrival_time - p_b->arrival_time);
    }
    else if (policy == sjf)
    {
        return (p_a->cpu_time_remaining - p_b->cpu_time_remaining);
    }
    else if (policy == priority)
    {
        return (p_b->priority - p_a->priority);
    }
    else
    {
        return (p_a->arrival_time - p_b->arrival_time);
    }

    printf("Unknown scheduling policy.\n");
    return 0;
}



char *get_policy()
{
    if (policy == fcfs)
    {
        return "FCFS";
    }
    else if (policy == sjf)
    {
        return "SJF";
    }
    else if (policy == priority)
    {
        return "PRIORITY";
    }

    return "NULL";
}


int set_policy(char *item)
{
	if (!strcmp(item, "fcfs"))
	{
		policy = fcfs;
		return 0;
	}
	else if (!strcmp(item, "sjf"))
	{
		policy = sjf;
		return 0;
	}
	else if (!strcmp(item, "priority"))
	{
		policy = priority;
		return 0;
	}

	else
	{
		printf("Policy error.\n");
		return 1;
	}

    return 1;
}


int get_wait_time()
{
    int wait = 0;
    int i;

    for (i = buffer_p; i > buffer_n; i++)
    {
        wait += running_processes[i]->cpu_time_remaining;
    }

    return wait;
}


void performance(int job_num, int is_test)
{
    int total_wait_time = 0;
    int total_turn_time = 0;
    int total_response_time = 0;
    int total_cpu_time = 0;

    new_process f_process;
    int index;

    for (index = 0; index < next_point; index++)
    {
        f_process = finished_processes[index];

        total_wait_time += f_process->cpu_time + f_process->waiting_time;
        total_turn_time += f_process->turnaround_time;
        total_response_time += f_process->response_time;
        total_cpu_time += f_process->cpu_time;
    }

    int finished_jobs = next_point + (buffer_n - buffer_p);
    if (is_test == 1)
    {
        finished_jobs = job_num;
    }

    printf("******************************************\n");
    printf("*Total number of jobs submitted: %d\n", finished_jobs);
    printf("*Average turnaround time: %.3f seconds\n", total_turn_time / (float)index);
    printf("*Average CPU time: %.3f seconds\n", total_cpu_time / (float)index);
    printf("*Average waiting time: %.3f seconds\n", total_wait_time / (float)index);
    printf("*Average response time: %.3f seconds\n", total_response_time / (float)index);
    printf("*Throughput: %.3f No./second\n", 1/(total_turn_time / (float)index));
    printf("******************************************\n");
}


void benchmark_ex(char *bench_name, int njobs, int t_arrival, int pri, int min_cpu, int max_cpu)
{
    if (!t_arrival)
    {
        batch_job = 1;
    }
    else
    {
        batch_job = 0;
    }

    int i;
    for (i = 0; i < njobs; i++)
    {
	    srand(1);
        if (i >= 100)
        {
            pthread_cond_signal(&buffer_not_empty);
        }

        pthread_mutex_lock(&count_mutex);

        while(progarm_wait == 100)
        {
            pthread_cond_wait(&buffer_not_full, &count_mutex);
        }

        pthread_mutex_unlock(&count_mutex);

        new_process p = malloc(sizeof(n_process));

        char new_name[99];
        int id = buffer_n;

        sprintf(new_name, "job%d", id);
    
        p->cpu_time = (rand() % (max_cpu + 1)) + 1;
        p->cpu_time_remaining = (rand() % (max_cpu + 1)) + 1;
        p->cpu_first_time = 0;
        p->priority = (rand() % (pri + 1)) + 1;
        p->response_time = 0;
        p->waiting_time = 0;
        p->turnaround_time = 0;
        p->finish_time = 0;
        p->arrival_time = time(NULL);
        strcpy(p->job_name, new_name);
        strcpy(p->program, "./soft");

        running_processes[buffer_n] = p;

        pthread_mutex_lock(&count_mutex);

        progarm_wait++;
        buffer_n++;

        sort_list(running_processes);
        buffer_n %= 100;

    pthread_mutex_unlock(&count_mutex);

        if (t_arrival)
        {
            pthread_cond_signal(&buffer_not_empty);
            sleep(t_arrival);
        }
    }

    if (!t_arrival)
    {
        pthread_cond_signal(&buffer_not_empty);
    }
}


void err_msg(char *item, bool err_flag)
{
    if (err_flag == true)
    {
        printf("%s", item);
        printf("\n");
    }
}
