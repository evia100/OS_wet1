//		commands.c
//********************************************
#include "commands.h"
#include <iostream>



//********************************************
// function name: signal_handler
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//*************************************************************************************

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//*************************************************************************************

int ExeCmd(smash& DB, char* lineSize, char* cmdString)
{
	DB.delete_dead_jobs();
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
		if (num_arg != 2 || args[1][0] != '-' )/* args[2])*/
			illegal_cmd = true;
		else
		{
			int job_id = atoi(args[2]);
			int signum = atoi(args[1]+1);
			list<job>::iterator it = DB.search_job(job_id);
			if(it==DB.jobs.end()) {
				cout <<"smash error: > kill " << job_id <<" ??? job does not exist" <<endl;
				return ERROR;
			}
			if(kill(it->pid,signum)==ERROR)
			{
				cout <<"smash error: > kill " << job_id <<" ??? cannot send signal" <<endl;
			}

			if(signum==SIGSTOP || signum==SIGTSTP)
			{
				it->curr_state=stopped; /////////////////////////// WHAT ABOUT THE TIME
			}
			else if (signum==SIGCONT)
			{
				it->curr_state=running; /////////////////////////// WHAT ABOUT THE TIME

			}

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
		if (num_arg >1)
			illegal_cmd = true;
		else
		{
			list<job>::iterator it;
			int job_id;
			if(num_arg==1)
				job_id=atoi(args[1]);
			else if(num_arg==0)
				job_id = DB.jobs.back().id;
			
			it=DB.search_job(job_id);
			if(it==DB.jobs.end())
			{
			cout <<"smash error: > no such job" <<endl;
			return ERROR;
			}
			if(it->curr_state==stopped)
			{
				int res = kill(it->pid,SIGCONT);
				if(res==ERROR) {
					cout <<"smash error: > can't get this job running" << endl;
				}
				else {
					it->curr_state = running;
				}
			}

			DB.PGID = it->pid;
		 	waitpid(it->pid,NULL,WUNTRACED);
			DB.PGID=-1;
		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "bg"))
	{
		if (num_arg >1)
			illegal_cmd = true;
		else
		{
			list<job>::iterator it;
			int job_id;
			if(num_arg==1)
				job_id=atoi(args[1]);
			else if(num_arg==0)
				job_id = DB.jobs.back().id;
			
			it=DB.search_job(job_id);
			if(it==DB.jobs.end() || it->curr_state==running)
			{
			cout <<"smash error: > no such job or active job" <<endl;
			return ERROR;
			}

			if(it->curr_state==stopped)
			{
				int res = kill(it->pid,SIGCONT);
				if(res==ERROR) {
					cout <<"smash error: > can't get this job running" << endl;
				}
				else {
					cout << job_id << endl;
					it->curr_state = running;
				}
			}
		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		if (num_arg > 1)
			illegal_cmd = true;
		else if(num_arg==0)
		{
			int pid = getpid();
			if(kill(pid,SIGKILL)==ERROR) {
				perror("couldn't kill smash");
			}
		}
		else if(num_arg==1) 
		{
			DB.delete_dead_jobs();
			// running over the jobs
			list<job>::iterator it;
			for (it=DB.jobs.begin();it!=DB.jobs.end();it++)
			{
				bool died = true;
				time_t start_time=time(0);
				if(kill(it->pid,SIGTERM)==ERROR) {
					perror("couldnt terminate job");
				}
				
				while(!kill(it->pid,0)) // job still exists
				{
					waitpid(-1,NULL,WNOHANG);
					int curr_time = (int)time(0);
					if((curr_time-(int)start_time) > 5) // job didnt die
					{
						died = false;
						if(kill(it->pid,SIGKILL)==ERROR) {
							perror("couldnt kill job");
						}
					}
				}
				if(died) {
					cout<< it->command <<" - Sending SIGTERM... Done." <<endl;
				}
				else {
					cout<< it->command <<" - Sending SIGTERM??? (5 sec passed) Sending SIGKILL??? Done." <<endl;
				}
			}
			int pid = getpid();
			if(kill(pid,SIGKILL)==ERROR) {
				perror("couldn't kill smash");
			}

		}
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
			FILE* File1, * File2;
			File1 = fopen(args[1], "r");
			File2 = fopen(args[2], "r");

			if (File1 == NULL || File2 == NULL) {
				perror("Could not open one of the files");
			}
			else
			{
				fseek(File1, 0, SEEK_END);
				long lsize1 = ftell(File1);
				rewind(File1);

				fseek(File2, 0, SEEK_END);
				long lsize2 = ftell(File2);
				rewind(File2);

				char* data1 = (char*)malloc(sizeof(char) * (lsize1 + 1));
				char* data2 = (char*)malloc(sizeof(char) * (lsize2 + 1));

				if (data1 == NULL || data2 == NULL)
				{
					perror("Could not allocate memory");
				}

				data1[lsize1] = '\0';
				data1[lsize2] = '\0';

				if (fread(data1, 1, lsize1, File1) != lsize1 || fread(data2, 1, lsize2, File2) != lsize2)
				{
					perror("error reading file");
				}
				if (strcmp(data1, data2))
					printf("1 \n");
				else
					printf("0 \n");

				free(data1);
				free(data2);
			}
			fclose(File1);
			fclose(File2);
		
		}

	}

	/*************************************************/
	else // external command
	{
		ExeExternal(args, DB);
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

void ExeExternal(char *args[MAX_ARG], smash& DB)
{
	int pID;
	pID = fork();
	switch(pID)
	{
		case ERROR:
		{
			perror("fork failed");
			break;
		}
		case 0 :
		{	// Child Process
			setpgrp();
			if(execvp(args[0],args)==ERROR) // failed
			{
				perror("can not run this command");
			}
			break;
		}
		default:
		{	
			DB.PGID = pID;
			waitpid(pID,NULL,WUNTRACED);
			DB.PGID=-1;
			break;
		}		
	}
}


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
			case ERROR: // fork failed
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
