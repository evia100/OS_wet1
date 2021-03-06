#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <list>
#include <queue>
#include <iostream>
#include <vector>
#include <errno.h>
#include <fstream>

using namespace std;

#define MAX_LINE_SIZE 80
#define MAX_ARG 20
#define HISTORY_SIZE 50
#define ERROR -1


enum State {running=0,stopped};


class job {

public:
job(std::string command,int pid,int id):command(command),pid(pid),id(id),curr_state(running),active_time(time(0)){};

std::string command;
int pid;
int id;
State curr_state;
time_t active_time;

};


class smash {
    public:

    char preivous_WD[MAX_LINE_SIZE];
    char current_WD[MAX_LINE_SIZE];
    int id;
    int PGID; // process group id
    char curr_cmd[MAX_LINE_SIZE];

    void string_swap(char* str1,char* str2)
    {
        char temp[MAX_LINE_SIZE];
        strcpy(temp ,str1);
        strcpy(str1 ,str2);
        strcpy(str2 ,temp);
    }


    std::vector <char*> commands_history;
    void queue_insert(char* command) 
    {
        cout << commands_history.size() << endl;
        if(commands_history.size()>HISTORY_SIZE)
        {
            commands_history.erase(commands_history.begin());
        }
        commands_history.push_back(command);
    }

    void queue_print()
    {

        vector<char*>::iterator it;
        for (it = commands_history.begin();it!=commands_history.end();it++)
        {
         cout << *it << endl;
        }
    }


    std::list <job> jobs;

    void delete_dead_jobs()
    {
        list<job>::iterator it;
        for (it=jobs.begin();it!=jobs.end();it++)
        {
            int status = kill(it->pid,0);  
            if(errno == ESRCH)
            {
                jobs.erase(it);
            }
        }
    }


    list<job>::iterator search_job(int job_id)
    {
        list<job>::iterator it;
        for (it=jobs.begin();it!=jobs.end();it++)
        {
            if(it->id == job_id)
            {
                return it;
            }
        }
        return jobs.end();
    }

    void print_jobs()
    {
        list<job>::iterator it;
        for (it=jobs.begin();it!=jobs.end();it++)
        {
            cout<<it->command << " : " << it->pid << " " << it->active_time <<endl; 
        }
    }

};



typedef enum { FALSE , TRUE } Bool;
int ExeComp(char* lineSize);
int BgCmd(char* lineSize, smash& DB);
int ExeCmd(smash& DB, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], smash& DB);

#endif

