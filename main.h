#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include<signal.h>
#ifndef COLORS_H
#define COLORS_H

#define  RED  	"\033[1;31m"
#define  GREEN	"\033[1;32m"
#define  BLUE 	"\033[1;34m"
#define  YELLOW	"\033[0;33m"
#define  PURPLE	"\033[0;35m"
#define  CYAN	"\033[0;36m"
#define  WHITE	"\033[0;37m"
#define  BLACK	"\033[0;30m"
#define  RESET	"\033[0m"
#endif

typedef struct job				//linked list structure
{
    int pid;
    int job_id;
    char cmd[50];
    int num;
    char status[50];
    struct job *link;
}jobs_list;

void echo_cmd(char *input_string, int status);
void internal_command(char *argv[], int argc, jobs_list **jobs);
void external_command(char *argv[], int argc, jobs_list **jobs);
void scan_input(char *prompt, char *input_string, jobs_list **jobs);
void signal_handler(int sig_num, siginfo_t *info, void *dummy);
int jobs_id_list(char *argv, int pid, int job_id, jobs_list **jobs);
void print_jobs(jobs_list **jobs);
int delete_jobs(jobs_list **jobs);
void fore_ground(jobs_list **jobs);
void back_ground(jobs_list **jobs);
int n_pipes(int argc, char *argv[]);
int get_num_of_pipes (char* argv []);
int validate_args (int argc, char* argv [], int* pos);
