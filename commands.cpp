//		commands.c
//********************************************
#include "commands.h"
#include <iostream>
#define ERROR -1

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(smash& DB, char* lineSize, char* cmdString)
{
	char* cmd;
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char temp[MAX_LINE_SIZE];
	strcat(temp,cmdString);
	char* delimiters = " \t\n";
	int i = 0, num_arg = 0;
	bool illegal_cmd = FALSE; // illegal command
	cmd = strtok(temp, delimiters);
	if (cmd == NULL)
		return 0;
	args[0] = cmd;
	for (i = 1; i < MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters);
		if (args[i] != NULL)
		{
			num_arg++;
		}

	}

	/*************************************************/
	// Built in Commands PLEASE NOTE NOT ALL REQUIRED
	// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
	// MORE IF STATEMENTS AS REQUIRED
	/*************************************************/
		/*************************************************/
	 if (!strcmp(cmd, "pwd"))
	{
		if (num_arg != 0) {
			illegal_cmd = true;
		}
		else
		{
			getcwd(DB.current_WD, MAX_LINE_SIZE);

			std::cout << DB.current_WD << std::endl;

		}
	}
	
	else if (!strcmp(cmd, "cd"))
	{
		if (num_arg == 1)
		{
			if (strcmp(args[1], "-") == 0) //TODO : check if we need to print even if is empty.
			{
				if (DB.preivous_WD[0] != '\0') // not empty
				{
					cout << DB.preivous_WD << endl;
					chdir(DB.preivous_WD);
					DB.string_swap(DB.preivous_WD, DB.current_WD);
				}
			}
			else // args[1]!="-"
			{
				if (chdir(args[1]) == ERROR)
				{
					cout << "smash error:> No such file or directory" << endl;
					return -1;
				}
				strcpy(DB.preivous_WD, DB.current_WD);
				getcwd(DB.current_WD, MAX_LINE_SIZE);

			}
		}
		else
			illegal_cmd = true;

	}


	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
		//not workig yet
		DB.queue_print();
	}

	/*************************************************/
	else if (!strcmp(cmd, "jobs"))
	{
		if (num_arg != 0)
			illegal_cmd = true;
		else
		{
			DB.delete_dead_jobs();
			DB.print_jobs();
		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		if (num_arg != 2)
			illegal_cmd = true;
		else
		{

		}
	}


	/*************************************************/
	//ADDED
	else if (!strcmp(cmd, "showpid"))
	{
		if (num_arg == 0)
		{
			int pid = getpid();
			std::cout << "smash pid is " << pid << std::endl;
		}
		else
		{
			illegal_cmd = true;
		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "fg"))
	{

	}

	/*************************************************/
	else if (!strcmp(cmd, "bg"))
	{

	}

	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{

	}

	/*************************************************/
	else if (!strcmp(cmd, "cp"))
	{
		FILE *inputFile,*outputFile;

		if (num_arg != 2)
			illegal_cmd = true;
		else
		{
			inputFile = fopen(args[1],"rb");
			outputFile = fopen(args[2],"wb");

			if(inputFile==NULL || outputFile==NULL) {
				perror("Could not open one of the files");
			}
			else 
			{
				fseek (inputFile , 0 , SEEK_END);
				long lsize = ftell(inputFile);
				rewind(inputFile);
				char* data = (char*) malloc (sizeof(char)*lsize);
				if(data==NULL)
				{
					perror("Could not allocate memory");
				}
				if(fread(data,1,lsize,inputFile)!=lsize)
				{
					perror("error reading file");	
				}
				if(fwrite(data,1,lsize,outputFile)!=lsize)
				{
					perror("error writing to file");	
				}
				free(data);
				cout << args[1] <<" has been copied to " << args[2] <<endl; 
			}
			fclose(inputFile);
			fclose(outputFile);
		}
	}

	else if (!strcmp(cmd, "diff"))
	{
		if (num_arg != 2)
			illegal_cmd = true;
		else
		{

		}

	}

	/*************************************************/
	else // external command
	{
		//ExeExternal(args, cmdString);
		return 0;
	}

	if (illegal_cmd == true)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
	return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
/*
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	int pID;
		switch(pID = fork())
	{
			case -1:
					// Add your code here (error)

					/*
					your code
					*/
					//	case 0 :
								// Child Process
					  //     		setpgrp();

								// Add your code here (execute an external command)

								/*
								your code
								*/

								//	default:
											// Add your code here

											/*
											your code
											*/
											//	}
											//}

											//**************************************************************************************
											// function name: ExeComp
											// Description: executes complicated command
											// Parameters: command string
											// Returns: 0- if complicated -1- if not
											//**************************************************************************************
//	}
//}
//NOT NEEDED
int ExeComp(char* lineSize)
{
	char ExtCmd[MAX_LINE_SIZE + 2];
	char* args[MAX_ARG];
	if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
	{
		// Add your code here (execute a complicated command)

		/*
		your code
		*/
	}
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, smash& DB)
{

	char* cmd;
	char* delimiters = " \t\n";
	char* args[MAX_ARG];
	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0;
	args[0] = cmd;
	int num_arg = 0;
	for (int i = 1; i < MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters);
		if (args[i] != NULL)
			num_arg++;

	}
	//////////// needs to add input test ///////////////
	int pid;
	if (lineSize[strlen(lineSize) - 2] == '&')
	{
		lineSize[strlen(lineSize) - 2] = '\0';
		// (execute a in the background)
		pid = fork();
		switch (pid)
		{
		case 0: // child
		{
			setpgrp();
			if (execvp(args[0], args) == ERROR)
			{
				perror("ERROR");
				exit(-1);
			}
			break;
		}
		case -1: // fork failed
		{
			perror("fork failed");
			break;
		}
		default: // continue 
		{
			string cmd(args[0]);
			job new_job(cmd, pid, ++DB.id);
			DB.jobs.push_back(new_job);
			return 0;
		}
		}
	}
	return -1;
}
