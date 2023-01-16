#include "main.h"
extern int job_flag, job_id, pid, background_flag;				
int jobs_id_list(char *argv, int pid, int job_id, jobs_list **jobs)		//function job id list
{
    jobs_list *new_node = malloc(sizeof(jobs_list));				//creating a new node with size of struct list
    if(new_node == NULL)							//if node has not been created print error
    {
	printf("Node is not configured\n");
	return -1;
    }
    new_node->link = NULL;							//updare the new node link with null and update all the value with he values
    new_node->job_id = job_id;
    new_node->pid = pid;
    strcpy(new_node->cmd,argv);
    if(job_id == 1)								//if there is only one job which has been terminated using ctrl z then make the job as null because it is making it go infinite loop 	
    {
	(*jobs) = NULL;
    }
    if(*jobs == NULL)						//if no jobs present ad the job directly
    {
	*jobs = new_node;
    }
    else
    {
	new_node->link = *jobs;					//else insert at first
	*jobs = new_node;
    }
}
int delete_jobs(jobs_list **jobs)				//function delete jobs
{
    if((*jobs)->link == NULL)
    {
	free(*jobs);					//if there is only one job delete by freeing the job
    }
    else
    {
	jobs_list *temp = *jobs;			//else delete at first
	*jobs = (*jobs)->link;
	free(temp);
	temp = NULL;
    }
}
void print_jobs(jobs_list **jobs)					//function print jobs
{
    int back_status;				
    if(background_flag == 1)						//if the background flag is 1 then using waitpd wuth pid of that job and wcontinued
    {
	if(waitpid((*jobs)->pid, &back_status, WCONTINUED) > 0)
	{
	    if(WIFEXITED(back_status))				//if it exited abnormally then run a loop to print the job and run ot and delete that job from the linked list
	    {
		jobs_list *temp = *jobs;	
		while(temp != NULL)
		{
		    printf("[%d] %d %s\t%s\n",(temp)->job_id,(temp)->pid,(temp)->status,(temp)->cmd);		//printing the list
		    temp = temp->link;
		}
		delete_jobs(jobs);
		job_id--;				
		background_flag = 0;
	    }
	    else 
	    {
		jobs_list *temp = *jobs;
		while(temp != NULL)
		{
		    printf("[%d] %d %s\t%s\n",(temp)->job_id,(temp)->pid,(temp)->status,(temp)->cmd);		//run a loop to print the num of jobs present
		    temp = temp->link;
		}
	    }
	}
    }
    else if(job_id < 1)
    {
	printf("No jobs available\n");				//if job id is less than 1 then print no jobs available
    }
    else
    {
	jobs_list *temp = *jobs;				//priniting all the job function
	while(temp!= NULL)
	{
	    printf("[%d]\t",(temp)->job_id);
	    printf("%d\t",(temp)->pid);
	    printf("%s\t",(temp)->status);
	    printf("%s\n",(temp)->cmd);
	    temp = temp->link;
	}
    }
}
