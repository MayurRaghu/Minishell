#include "main.h"
int pipe_cnt;
/* Function to execute n pipes */
int n_pipes(int argc, char *argv[])
{
      argv[argc] = NULL;
	if (argc < 3)	//The minimum number of Command Line Arguments shall be 4 for the Program to execute.
	{
		printf ("ERROR: Invalid number of Command Line Arguments passed.\n");
		printf ("Usage: ./pipe <command1> '|' <command2>\nUsage: ./pipe <command1> '|' <command2> '|' <command3> etc\n");
		return 0;
	}
	else			//If the number of Command Line Arguments are greater than 4, check for the number of Pipes.
	{

		pipe_cnt = get_num_of_pipes (argv);		//To Count the Number of Pipes in the CLA.
		printf ("The Number of Pipes in the passed CLA = %d\n", pipe_cnt);
		if (pipe_cnt < 1)	//If there are no Pipes in the CLA, display the Error message.
		{
			printf ("ERROR: Invalid Command Line Arguments passed.\n");
			printf ("Usage: ./pipe <command1> '|' <command2>\nUsage: ./pipe <command1> '|' <command2> '|' <command3> etc\n");
			return 0;
		}
		else				//Otherwise, check if the CLA are provided correctly or not and store the Pipe positions in CLA.
		{
			int pipe_pos [pipe_cnt];	//To store the n-Pipe positions in the CLA.

			/* If the Command Line Arguments are not provided in a correct way, display an Error message */
			if ((validate_args (argc, argv, pipe_pos)) == 0)
			{
				printf ("Usage: ./pipe <command1> '|' <command2>\nUsage: ./pipe <command1> '|' <command2> '|' <command3> etc\n");
				return 0;
			}
			else	//Otherwise, proceed for the executtion of the Commands passed in CLA.
			{
				/* To create the n-Pipes */
				int pipe_fd [pipe_cnt][2];
				for (int i = 0; i < pipe_cnt; i++)
				{
					pipe (pipe_fd [i]);
				}
				printf ("n-Pipes created successfully.\n");

				/* If there are 'n' Pipes in CLA; there shall be (n+1) Commands in CLA */
				int pid;
				for (int i = 0; i < (pipe_cnt + 1); i++)
				{
					if (i > 0)	//From the Second Pipe onwards till the nth Pipe.
					{
						close (pipe_fd [i - 1][1]);		//Close the Write end of the Previous Pipe.

						if (i != pipe_cnt)	//For all the Commands except the Last Command.
						{
							dup2 (pipe_fd [i - 1][0], 0);	//To Replace the Standard Input with the Read end of the Previous Pipe.
						}
					}
					pid = fork ();	//To create the Child Process.
					if (pid > 0)	//If the 'pid' is greater than 0; it is the Parent Process.
					{
						//The Parent Process shall not execute any Commands.
					}
					else if (pid == 0)	//If the 'pid' is 0; it is the Child Process.
					{
						if (i == 0)		//If it is the First Child Process.
						{
							close (pipe_fd [0][0]);			//Close the Read end of the 1st Pipe.
							dup2 (pipe_fd [0][1], 1);		//To Replace the Standard Output with the Write end of the 1st Pipe.
							argv [pipe_pos [0]] = NULL;		//To terminate the First Command in CLA.

							/* Execute the First Command */
							if ((execvp (argv [0], argv)) == -1)
							{
								perror ("execvp");
								return 0;
							}
						}
						else if (i == pipe_cnt)		//If it is the Last Child Process.
						{
							close (pipe_fd [pipe_cnt - 1][1]);		//Close the Write end of the Last Pipe.
							dup2 (pipe_fd [pipe_cnt - 1][0], 0);	//To Replace the Standard Input with the Read end of the Last Pipe.

							/* Execute the Last Command */
							if ((execvp ((argv [pipe_pos [pipe_cnt - 1] + 1]), (argv + pipe_pos [pipe_cnt - 1] + 1))) == -1)
							{
								perror ("execvp");
								return 0;
							}
							printf ("Last Command executed\n");
						}
						else	//If it is any of the Middle Child Processes.
						{
							close (pipe_fd [i][0]);			//Close the Read end of the Current Pipe.
							dup2 (pipe_fd [i][1], 1);		//To Replace the Standard Output with the Write end of the Current Pipe.
							argv [pipe_pos [i]] = NULL;		//To terminate the ith Command in CLA.

							/* Execute the Command starting from the Previous Pipe position to the NULL termination of the CLA */
							if ((execvp ((argv [pipe_pos [i - 1] + 1]), (argv + pipe_pos [i - 1] + 1))) == -1)
							{
								perror ("execvp");
								return 0;
							}
						}
					}
					else	//Error Handling.
					{
						printf ("The Child Process is not created.\n");
					}
				}
			}

			for (int i = 0; i < (pipe_cnt + 1); i++)	//The Parent shall wait for all the Child Processes to terminate.
			{
				wait (NULL);
			}
		}
	}

	return 0;
}

int get_num_of_pipes (char* argv [])
{
    int count = 0, pos = 1;

    while (argv [pos] != NULL)	//The loop shall run till all the CLAs are checked.
    {
	if ((strcmp (argv [pos], "|")) == 0)	//If a Pipe is encountered, the 'count' shall be incremented by 1.
	    count += 1;
	pos += 1;
    }

    return count;	//Return the Number of Pipes encountered.
}

int validate_args (int argc, char* argv [], int* pos)
{
    if ((strcmp (argv [0], "|")) == 0)			//The First CLA cannot be a Pipe; it has to be a Command.
    {
	printf ("ERROR: The first Command Line Argument cannot be a Pipe.\n");
	return 0;
    }
    if ((strcmp (argv [argc - 1], "|")) == 0)	//The Last CLA cannot be a Pipe; it has to be a Command.
    {
	printf ("ERROR: The last Command Line Argument cannot be a Pipe.\n");
	return 0;
    }

    int i = 1, j = 0;	//To traverse the CLA and track the position of the 'pos' Array.
    while (i < argc)	//The loop shall run till all the CLAs are checked.
    {
	/* If a Pipe is found and the previous CLA is not a Pipe; it constitutes a valid set of Commands to be executed */
	if (((strcmp (argv [i], "|")) == 0) && ((strcmp (argv [i - 1], "|")) != 0))
	{
	    pos [j++] = i;
	}
	else if (((strcmp (argv [i], "|")) == 0) && ((strcmp (argv [i - 1], "|")) == 0))
	{
	    printf ("ERROR: Consecutive Pipes found at positions %d and %d in the Command Line Arguments.\n", i , (i-1));
	    return 0;
	}
	i+= 1;
    }

    return 1;
}
