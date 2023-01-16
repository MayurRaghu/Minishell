#include "main.h"
int flag;								//take a global variable flag which will decide whether ctrl z or ctrl c is pressed
int main(int argc, char *argv[])				
{
    system("clear");							//using system to clear
    char input[50];
    char prompt[50] = "minishell";					//prompt minishell
    jobs_list *jobs = NULL;						//making the head of the linked list as null
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;						//declaring flags
    act.sa_sigaction = signal_handler;					//and in sigaction signal handler

    sigaction(SIGINT, &act, NULL);					//using sigaction with ctrl c and ctrl z and struct of sigaction act and null
    sigaction(SIGTSTP, &act, NULL);

    scan_input(prompt, input, &jobs);
}
extern int pid, job_id;
void signal_handler(int sig_num, siginfo_t *info, void *dummy)			//signal handler function
{
    switch(sig_num)						//case signum
    {
	case 2:								//if 2 which means sigint - ctrl c to terminate
	    flag = 1;
	    printf("\n");
	    break;
    	case 20:	    //case 20 which means sigtstp - ctrl z to stop the process
	    printf("\n");
	    if(job_id > 0)
	    {
		kill(pid, SIGTSTP);					//to kill the process tstp
	    	flag = 2;
	    }
	    break;
    }
}
