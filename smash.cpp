/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20

char* L_Fg_Cmd;
//void* jobs = NULL; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
char lineSize[MAX_LINE_SIZE]; 

smash DB; // smash data base

//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv
	[])
{
    char cmdString[MAX_LINE_SIZE]; 	   

	//signal declaretions
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	 /* add your code here */
	
	/************************************/
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	//set your signal handlers here
	/* add your code here */

	/************************************/
	// smash init
	DB.id=0;
	DB.preivous_WD[0]='\0';
	getcwd(DB.current_WD, MAX_LINE_SIZE);

	/************************************/
	// Init globals 
	
	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
	if (L_Fg_Cmd == NULL) 
			exit (-1); 
	L_Fg_Cmd[0] = '\0';

	while (1)
	{
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);    	
		cmdString[strlen(lineSize)-1]='\0';

		if (!ExeComp(lineSize)) continue;
					// background command	
	 	if(!BgCmd(lineSize, DB)) continue; 
					// built in commands
		ExeCmd(DB, lineSize, cmdString);

				//add to history
	//	cout << "line size is: " << lineSize << endl;
		if (strcmp(lineSize, "history")) {
			DB.commands_history.push_back(lineSize);
		//	cout <<"inserted : " << DB.commands_history.front() << endl;
		}

		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
    return 0;
}

