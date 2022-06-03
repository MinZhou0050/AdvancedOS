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


void main(int argc, char *argv[])
{
    pid_t child, pid;
    int count = 0;
    child = fork();

    if (child < 0)
    {
        printf("New process has faile.\n");
    }
    else if (child == 0)
    {
        sleep(atoi(argv[1]));
        exit(0);
    }
    while ((pid = wait(&count)) > 0){}
}
