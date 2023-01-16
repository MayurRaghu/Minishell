#include "main.h"
int status, job_id, pid, pipe_flag;
extern int flag, pipe_cnt;
int path_flag;
char pathname[5000] = {'\0'};
void scan_input(char *prompt, char *input_string, jobs_list **jobs)
{
    while(1)
    {
    	char *argv[10];							//for storing the argument using 2d array
	memset(input_string, 0, 50);					//for clearing the input string
	int ext_flag = 0, int_flag = 0;
	char *ext_cmd[2000];						//for external command
	int fd;
	int argc = 0;
	int i = 0, j = 0,k = 0;
	char *int_cmd[8] = {"cd", "pwd", "mkdir", "rmdir", "exit", "jobs", "bg", "fg"};			//internal command
	for(i = 0; i < 2000; i++)
	{
	    ext_cmd[i] = calloc(sizeof(char), 20);						//allocating memory for external command and argv
	}
	for(i = 0; i < 10; i++)
	{
	    argv[i] = malloc(sizeof(char)*5);
	}
	fflush(stdin);
	char cwd[5000] = {'\0'};
	printf(GREEN"%s:"BLUE"%s"WHITE"$ ",prompt, getcwd(cwd, sizeof(cwd)));				//printing the prompt	
	printf(WHITE);
	fgets(input_string, 50, stdin);					//to take input from stdin using fgets because while entering it was taking the null character included 
	int len = strlen(input_string) - 1;				//to find the string len
	input_string[len] = '\0';
	i = 0, j = 0, k = 0;
	//to store the argument from the cla to the array argv
	while(input_string[i] != '\0')			
	{
	    if(input_string[i] == ' ')			//if suppose a argument is passed ls -l there is space so ls is stored in first row and -l is stored in the 2nd row	
	    {
		argv[j][k] = '\0';
		k = 0;
		i++;
		j++;
		argc++;
		continue;
	    }
	    argv[j][k] = input_string[i];	
	    i++;
	    k++;
	}
	argv[j][k] = '\0';
	argc++;					//increment the varialble argument count to find the num of arguments passed and stored in the command	line suppose ls -l is passed the the argument count is 2
	if(strncmp(input_string,"PS1=",4) == 0 && input_string[4] != ' ')		//to change the prompt name PS1 is used so comparing the cla with PS1 if true then copy the new name from the command line to the prompt
	{
	    strcpy(prompt, input_string + 4);
	}
	else if(strcmp(input_string, "echo $$") == 0||strcmp(input_string, "echo $?")==0 || strcmp(input_string, "echo $SHELL")==0)		//fpr echo command call the function echo_cmd with the input string
	{
	    echo_cmd(input_string, status);
	}
	else if(input_string[0] != '\0')		//this part is for to check whether it is a internal command or and external command
	{
	    if(path_flag == 0)
	    {
		strcpy(pathname, cwd);
		strcat(pathname,"/file.txt");
		path_flag = 1;
	    }
	    fd = open(pathname,O_RDONLY);		//open the file.txt which has all the external commands it is copied to file using command ls /bin > file.txt	
	    if(fd == -1)				//error handling
	    {
		if(errno == EEXIST)
		{
		    perror("open");
		}
	    }
	    char str;
	    int ret, m = 0, n = 0;
	    while(read(fd, &str, 1) > 0)		//storing alll the external commands present in file.txt to the 2d array ext_cmd
	    {
		if(str == '\n')
		{
		    m++;
		    n = 0;
		    continue;
		}
		ext_cmd[m][n++] = str;
	    }
	    close(fd);				//close the file.txt
	    for(i = 0; i < 2000;i++)		//running the loop to check if the command is exernal or internal
	    {
		if(strcmp(ext_cmd[i], argv[0]) == 0)
		{		
		    ext_flag = 1;		
		    break;	    //if external make the ext flag as 1 and if internal then make int flag as 1
		}
		else if((i < 8) && (strcmp(argv[0], int_cmd[i]) == 0))
		{
		    int_flag = 1;
		    break;
		}
		else if((strcmp(argv[2],"|")==0) || (strcmp(argv[1], "|") == 0))
		{
		    pipe_flag = 1;
		    break;
		}
	    }
	    if(pipe_flag == 1)			//if pipe flag is 1 then call the n_pipes function
	    {
		n_pipes(argc, argv);
		if(pipe_cnt > 1)
		{
		    break;			//doing this because the more than 1 pipe is getting an infinite loop so using break
		}
		pipe_flag = 0;
	    }
	    else if(ext_flag == 1)			//if ext flag is 1 then call ext_command and if int flag is 1 thn call int command
	    {
		external_command(argv, argc, jobs);
	    }
	    else if(int_flag == 1)
	    {
		internal_command(argv,argc, jobs);
	    }
	    else
	    {
		printf("%s: Command not found\n",*argv);		//else command not found
	    }
	}
    }
}
void external_command(char *argv[], int argc, jobs_list **jobs)			//function external command
{
    argv[argc] = NULL;						//???????????????	
    pid = fork();						//creating a parent chil procees using fork and storing the parent id 
    if(pid > 0)							//if it is a parent process 
    {
	job_id++;						//increment the job_id
	jobs_id_list(argv[0], pid, job_id, jobs);		//adding it to the linked list
	int w_pid;						
	if((w_pid = waitpid(pid, &status, WUNTRACED))>0)	//using waitpid with status pid and WUNTRACED - it returns if the child has stopped
	{
	    if(WIFEXITED(status))				//if exited normally that means sigint/sigtstp is not used if that is true and to that particular job id delete that job from the linked list and decrement the job id
	    {
		if(job_id > 0)
		{
		    delete_jobs(jobs);
		    job_id--;
		}
	    }
	}
	else if(w_pid == -1)					//if waitpid is equal to -1 then it means that the process has ended abruptly if thats the case then it will
	{

	    if(errno == EINTR && flag == 2)			//if the errno is EINTR that means an unblocked signal or a sigchild is caught and the flag is 2 (flag is the extern flag from the signal handler of the sigtstp) then it means ctrl z is pressed which means the function should get stopped
	    {
		strcpy((*jobs)->status,"stopped");		//store stopped in the status of the structure of the linked list
		printf("\n[%d] %s\t%s\n",job_id,(*jobs)->status,(*jobs)->cmd);
	    }
	    else if(flag == 1 && errno == EINTR)		//if flag is 1 then it means ctrl c is pressed which means that sigint - the program will get terminated
	    {
		delete_jobs(jobs);				//if true then delete the job from the linked list and decrement the job id
		job_id--;
	    }
	    flag = 0;						//and make that flag as 0
	}
    }
    else
    {
	execvp(argv[0], argv);					//in child process by using execvp printing the desired output to the stdout
    }
}
void internal_command(char *argv[], int argc, jobs_list **jobs)		//function internal command
{
    if(strcmp(argv[0], "mkdir")==0)					//mkdir funcion
    {
	mkdir(argv[1], 0664);
    }
    else if(strcmp(argv[0], "rmdir") == 0)				//rmdir
    {
	rmdir(argv[1]);
    }
    else if(strcmp(argv[0], "exit") == 0)			//for exit status
    {
	exit(2);
    }
    else if(strcmp(argv[0], "pwd") == 0)			//pwd
    {
	char buf[100];
	getcwd(buf,100);
	printf("%s\n",buf);
    }
    else if(strcmp(argv[0], "jobs") == 0)		//to print jobs
    {
	print_jobs(jobs);
    }
    else if(strcmp(argv[0], "fg") == 0)			//to run fore ground
    {
	fore_ground(jobs);
    }
    else if(strcmp(argv[0], "bg") == 0)
    {
	back_ground(jobs);				//background
    }
    else if(strcmp(argv[0], "cd") == 0)
    {
	if(chdir(argv[1]) == -1)
	{
	    perror("bash");
	}	 					//cd

    }
}
void echo_cmd(char *input_string, int status)		//echo command
{
    if(strcmp(input_string, "echo $$")==0)
    {
	printf("%d\n",getpid());
    }
    else if(strcmp(input_string, "echo $SHELL")==0)
    {
	printf("/bin/bash\n");
    }
    else if(strcmp(input_string, "echo $?")==0)
    {
	if(WIFEXITED(status))
	    printf("%d\n", WEXITSTATUS(status));
    }
}
