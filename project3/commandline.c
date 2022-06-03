// Name: Min Zhou
// Date: 3/20/2022
// Referenced code from Professor.Qin

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>

#include "commandline.h"
#include "schedule.h"


int quit(int nargs, char **args) {
	
	if (strcmp(args[1],"-d"))
	{
		if (progarm_wait != 0)
		{
			printf("Waiting...\n");
			while (progarm_wait > 0){ }
		}
	}
	else if (strcmp(args[1],"-i"))
	{
		performance(0,0); 
		exit(0);
	}
	
	performance(0,0); 
	exit(0);
}


int process_list(int nargs, char **args)
{
	if (progarm_wait > 0)
	{
		int i;
		char *j_status;
		struct tm time2;

		char* curr_policy = get_policy(); 
		printf("*********************************************\n");
		printf("*Total number of jobs in the queue: %d\n", buffer_n - buffer_p);
		printf("*Scheduling Policy: %s\n", curr_policy);
		printf("*Name\tCPU_Time\tPri\tArrival_time\tProgress\n");

		for (i=0; i < buffer_n; i++)
		{
			j_status = "";

			if (running_processes[i]->cpu_time_remaining == 0)
			{
				continue;
			}
			else if (running_processes[i]->cpu_first_time > 0 && running_processes[i]->cpu_time_remaining > 0)
			{
				j_status = "Run";
			}

			time2 = *localtime(&running_processes[i]->arrival_time);
			
			printf("%s\t%d\t\t%d\t%02d:%02d:%02d\t%s\t\n", 
			running_processes[i]->job_name, 
			running_processes[i]->cpu_time, 
			running_processes[i]->priority, 
			time2.tm_hour,
			time2.tm_min,
			time2.tm_sec,
			j_status);
		}
		printf("*********************************************\n");
	}

	else
	{
		printf("Nothing is running.\n");
	}

	return 0;
}


static const char *helpmenu[] = {
	"run <job> <time> <pri>: submit a job named <job>,\n\t\t\texecution time is <time>,\n\t\t\tpriority is <pri>.\n",
	"list: display the job status.\n",
	"fcfs: change the scheduling policy to FCFS.\n",
	"sjf: change the scheduling policy to SJF.\n",
	"priority: change the scheduling policy to priority.\n",
	"test <benchmark> <policy> <num_of_jobs> <arrival_rate>\n     <priority_levels> <min_CPU_time> <max_CPU_time>\n",
	"quit: exit AUbatch\n",
	NULL
};


void showmenu(const char *x[])
{
	int i;

	printf("\n");
	
	for (i=0; x[i]; i++) {
		printf("%s", x[i]);
	}
	printf("\n");
}


int help(int n, char **a)
{
	(void)n;
	(void)a;

	showmenu(helpmenu);
	return 0;
}



int run_fcfs(int nargs, char **args)
{
	policy = fcfs;
	printf("Scheduling policy is switched to FCFS. All the %d waiting jobs have been rescheduled.\n", (buffer_n - buffer_p)-1);

	if (progarm_wait)
	{
		sort_list(running_processes);
	}

	return 0;
}


int run_sjf(int nargs, char **args)
{
	policy = sjf;
	printf("Scheduling policy is switched to SJF. All the %d waiting jobs have been rescheduled.\n", (buffer_n - buffer_p)-1);

	if (progarm_wait)
	{
		sort_list(running_processes);
	}

	return 0;
}


int run_pri(int nargs, char **args)
{
	policy = priority;
	printf("Scheduling policy is switched to PRIORITY. All the %d waiting jobs have been rescheduled.\n", (buffer_n - buffer_p)-1);

	if (progarm_wait)
	{
		sort_list(running_processes);
	}

	return 0;
}


int bench(int nargs, char **argv)
{
	if (nargs != 8)
	{
		printf("Please input: test <benchmark> <policy> <num_of_jobs> <arrival_rate> <priority_levels> <min_CPU_time> <max_cpu_time>\n");
		return(1);
	}
	else if (progarm_wait)
	{
		printf("Processes are busy.\n");
		return(1);
	}

	char *bench_name = argv[1];
	char *policy_name = argv[2];
	int num_jobs = atoi(argv[3]);
	int arrival_rate = atoi(argv[4]);
	int priority_lvl = atoi(argv[5]);
	int minc = atoi(argv[6]);
	int maxc = atoi(argv[7]);

	if ((minc >= maxc) || (num_jobs <= 0 || minc < 0 || maxc < 0 || priority_lvl < 0 || arrival_rate < 0))
	{
		printf("Min CPU time cannot be bigger than or equal to Max CPU time.\n");
		printf("Initial benchmark variables cannot be less than zero.\n");
		return(1);
	}
	
	int policy_result = set_policy(policy_name);

	if (policy_result == 1)
	{
		return(1);
	}

	benchmark_ex(bench_name, num_jobs, arrival_rate, priority_lvl, minc, maxc);

	while(progarm_wait) { }

	performance(num_jobs, 1);

	return 0;
}


int run(int nargs, char **args) {

	if (nargs != 4) {
		printf("Please input: run <job> <time> <priority>\n");
		return(1);
	}   

	FILE *file1 = fopen(args[1], "r");
	if (file1 == NULL)
	{
		printf("File does not exist.\n");
		return(1);
	}

	fclose(file1);

	scheduler(nargs, args);

	return 0;
}


static struct {
	const char *name;
	int (*func)(int nargs, char **args);
} cmdtable[] = {
	{ "?\n",	help },
	{ "h\n",	help },
	{ "help\n",	help },
	{ "r",		run },
	{ "run",	run },
	{ "list\n",	process_list },
	{ "l\n",	process_list },
	{ "fcfs\n",	run_fcfs },
	{ "sjf\n",	run_sjf },
	{ "priority\n",	run_pri },
	{ "test",	bench },
	{ "q\n",	quit },
	{ "quit",	quit },
	{ "quit\n",	quit },
    {NULL, NULL}
};



int assing(char *cmd)
{
	char *args[8];
	int nargs=0;
	char *word;
	char *context;
 	int i, result;

	for (word = strtok_r(cmd, " ", &context);
	     word != NULL;
	     word = strtok_r(NULL, " ", &context)) {

		if (nargs > 8) {
			printf("Command line has too many words\n");
			return(2);
		}
		args[nargs++] = word;
	}

	if (nargs==0) {
		return 0;
	}

	for (i=0; cmdtable[i].name; i++) {
		
		if (*cmdtable[i].name && !strcmp(args[0], cmdtable[i].name)) {
			assert(cmdtable[i].func!=NULL);
			
			result = cmdtable[i].func(nargs, args);

			return result;
		}
	}

	printf("%s: Command not found\n", args[0]);
	return(1);
}


void *commandline( void *ptr )
{
	    char *buffer;
        size_t bufsize = 64;
        
        buffer = (char*) malloc(bufsize * sizeof(char));

        if (buffer == NULL) 
		{
 			perror("Unable to malloc buffer");
 			exit(1);
		}

    	while (1) 
		{
			printf("\n>");
			getline(&buffer, &bufsize, stdin);
			assing(buffer);
		}

        return 0;
}

