// Name: Min Zhou
// Date: 3/20/2022
// Referenced code from Professor.Qin

void menu(char *line, int isargs); 
void showmenu(const char *x[]);
int help(int n, char **a);
int quit(int nargs, char **args); 
int assing(char *cmd);
void *commandline( void *ptr );

int run(int nargs, char **args);
int process_list(int nargs, char **args);
int bench(int nargs, char **argv);

int run_fcfs(int nargs, char **args);
int run_sjf(int nargs, char **args);
int run_pri(int nargs, char **args);