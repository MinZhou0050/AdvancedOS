// Name: Min Zhou
// Date: 3/20/2022
// Referenced code from Professor.Qin

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdbool.h>

#include "commandline.h"
#include "schedule.h"

int main(int argc, char *argv[])
{
  // welcom menu
  printf("****************************************************\n");
  printf("*Welcome to Min's batch job scheduler Version 1.0\n");
  printf("*Type 'help' to find more about AUbatch commands.\n");
  printf("****************************************************\n");

  //code from Professor.Qin
  pthread_t command_thread, executor_thread;

  char *message1 = "Command Thread";
  char *message2 = "Executor Thread";
  int  iret1, iret2;

  //global para
  progarm_wait = 0; 
  buffer_n = 0;  
  buffer_p = 0;
  err_flag = 0; 
  batch_job = 0;
  policy = fcfs;

  iret1 = pthread_create(&command_thread, NULL, commandline, (void*) message1);
  iret2 = pthread_create(&executor_thread, NULL, dispatch, (void*) message2);

  pthread_mutex_init(&count_mutex, NULL);
  pthread_cond_init(&buffer_not_full, NULL);
  pthread_cond_init(&buffer_not_empty, NULL);
    
  pthread_join(command_thread, NULL);
  pthread_join(executor_thread, NULL); 

  printf("command_thread returns: %d\n",iret1);
  printf("executor_thread returns: %d\n",iret2);
  exit(0);

  return 0;
}


