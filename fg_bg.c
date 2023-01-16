#include "main.h"
int background_flag;
extern int job_id, status;
void fore_ground(jobs_list **jobs)					//function foreground
{
    tcsetpgrp(1, getpgid((*jobs)->pid));				//using this to wait until the job is completed
    kill((*jobs)->pid, SIGCONT);
    if(waitpid((*jobs)->pid, &status, WUNTRACED)>0)
    {
	if(WIFEXITED(status))
	{
	    delete_jobs(jobs);						//then deleting the job
	    job_id--;
	}
    }
}
void back_ground(jobs_list **jobs)					//background
{
    if((*jobs) == NULL)
    {
	printf("No jobs available\n");
    }
    else
    {
	printf("[%d]+ %s &\n",(*jobs)->job_id, (*jobs)->cmd); 			//to run it in the background
	strcpy((*jobs)->status,"running");					
	kill((*jobs)->pid, SIGCONT);						//to kill the id
	background_flag = 1;							//make the flag as 1
    }
}
