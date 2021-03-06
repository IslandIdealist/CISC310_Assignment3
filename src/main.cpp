#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include "configreader.h"
#include "process.h"
#include <time.h>
#include <bits/stdc++.h>

// Shared data for all cores
typedef struct SchedulerData {
    std::mutex mutex;
    std::condition_variable condition;
    ScheduleAlgorithm algorithm;
    uint32_t context_switch;
    uint32_t time_slice;
    std::list<Process*> ready_queue;
    bool all_terminated;
} SchedulerData;

void coreRunProcesses(uint8_t core_id, SchedulerData *data);
int printProcessOutput(std::vector<Process*>& processes, std::mutex& mutex);
void clearOutput(int num_lines);
#include<iterator>


uint32_t currentTime();
std::string processStateToString(Process::State state);

int main(int argc, char **argv)
{
    // ensure user entered a command line parameter for configuration file name
    if (argc < 2)
    {
        std::cerr << "Error: must specify configuration file" << std::endl;
        exit(1);
    }

    // declare variables used throughout main
    int i;
    SchedulerData *shared_data;
    std::vector<Process*> processes;

    // read configuration file for scheduling simulation
    SchedulerConfig *config = readConfigFile(argv[1]);

    // store configuration parameters in shared data object
    uint8_t num_cores = config->cores;
    shared_data = new SchedulerData();
    shared_data->algorithm = config->algorithm;
    shared_data->context_switch = config->context_switch;
    shared_data->time_slice = config->time_slice;
    shared_data->all_terminated = false;

    // create processes
    uint32_t start = currentTime();
    for (i = 0; i < config->num_processes; i++)
    {
        Process *p = new Process(config->processes[i], start);
        processes.push_back(p);
        if (p->getState() == Process::State::Ready)
        {
            shared_data->ready_queue.push_back(p);
        }
    }

    // free configuration data from memory
    deleteConfig(config);

    // launch 1 scheduling thread per cpu core
    std::thread *schedule_threads = new std::thread[num_cores];
    for (i = 0; i < num_cores; i++)
    {
        schedule_threads[i] = std::thread(coreRunProcesses, i, shared_data);
    }

	unsigned int startTime = currentTime();

    // main thread work goes here:
    int num_lines = 0;
    while (!(shared_data->all_terminated))
    {

		std::vector<Process*>::iterator itr;
		std::vector<Process*>::iterator nestedItr;
        // clear output from previous iteration
        clearOutput(num_lines);

        // start new processes at their appropriate start time

		for(itr = processes.begin(); itr < processes.end(); itr++){
			if((*itr)->getState() == Process::State::NotStarted && (*itr)->getStartTime() + startTime < currentTime()){ //Is process not started and is it time to start it?

				(*itr)->setState(Process::State::Ready, currentTime()); //Set state to ready
				shared_data->ready_queue.push_back(*itr); //Put back on ready queue
				
				printf("Process added to ready queue.\n");
			}
		}

        // determine when an I/O burst finishes and put the process back in the ready queue

		for(itr = processes.begin(); itr < processes.end(); itr++){
			if((*itr)->getState() == Process::State::IO && (*itr)->getCurrBurst()+ (*itr)->getEntryTime() < currentTime()){ //Is process in I/O and has elapsed time passed?

				(*itr)->setState(Process::State::Ready, currentTime()); //Set state back to ready
				(*itr)->incrementCurrBurst(); //Move to next index of bursts
				shared_data->ready_queue.push_back(*itr); //Put back on ready queue

				printf("Process I/O burst done; added back to ready queue.\n");
			}
		}

        // sort the ready queue (if needed - based on scheduling algorithm)

		for(itr = processes.begin(); itr < processes.end(); itr++){
			for(nestedItr = processes.begin(); nestedItr < processes.end(); nestedItr ++){

				//if(SjfComparator::operator ()(*nestedItr, *(nestedItr+1)){
				//This is where I finished up for the night.
				//}		
			}
		}
		/*Stopping point*/


        // determine if all processes are in the terminated state

        // output process status table
        num_lines = printProcessOutput(processes, shared_data->mutex);

        // sleep 1/60th of a second
        usleep(16667);
    }


    // wait for threads to finish
    for (i = 0; i < num_cores; i++)
    {
        schedule_threads[i].join();
    }

    // print final statistics
    //  - CPU utilization
    //  - Throughput
    //     - Average for first 50% of processes finished
    //     - Average for second 50% of processes finished
    //     - Overall average
    //  - Average turnaround time
    //  - Average waiting time


    // Clean up before quitting program
    processes.clear();

    return 0;
}

void coreRunProcesses(uint8_t core_id, SchedulerData *shared_data)
{
    // Work to be done by each core idependent of the other cores
    //  - Get process at front of ready queue
    //  - Simulate the processes running until one of the following:
    //     - CPU burst time has elapsed
    //     - RR time slice has elapsed
    //     - Process preempted by higher priority process
    //  - Place the process back in the appropriate queue
    //     - I/O queue if CPU burst finished (and process not finished)
    //     - Terminated if CPU burst finished and no more bursts remain
    //     - Ready queue if time slice elapsed or process was preempted
    //  - Wait context switching time
    //  * Repeat until all processes in terminated state

	//currentProcess = new std::Process();	

	/*
		Need to increment current burst index after CPU calculation is done
		When burst is done, make sure that the entry time is set to current time so that it can be later added back to ready queue.

	*/


/*	currentProcess = *shared_data.ready_queue.pop_front(); //pop the top item off the queue
	
	auto start = currentTime(); //start the timing
	while( processChecker == false )
	{
		if( *shared_data.ready_queue.end().pid < currentProcess.pid ) //check if the top of the queue has a higher priorty than the current running process
		{
				auto stop = currentTime(); //stop the clock
				auto timePassed = (stop - start); //generate the time duration
				currentProcess.wait_time = timePassed; //sets the amount of time that has been completed so far
				currentProcess.state = State::Ready;  //sets the current process back to ready
				*shared_data.ready_queue.insert(currentProcess); //inserts the current process back into the queue
				currentProcess = *shared_data.ready_queue.pop_front(); //pops the new "higher" priority process off the stack
			
		}
		
		if( currentProcess.burst_times[currentProccess.current_burst] == currentProcess.wait_time ) //checks to see if the process has completed
		{
			
			currentProcess.State = State::IO; //sets the current process to IO state
			processChecker == true; //breaks out of the loop

		}
		
		
		
	} */
		

	
}

int printProcessOutput(std::vector<Process*>& processes, std::mutex& mutex)
{
    int i;
    int num_lines = 2;
    std::lock_guard<std::mutex> lock(mutex);
    printf("|   PID | Priority |      State | Core | Turn Time | Wait Time | CPU Time | Remain Time |\n");
    printf("+-------+----------+------------+------+-----------+-----------+----------+-------------+\n");
    for (i = 0; i < processes.size(); i++)
    {
        if (processes[i]->getState() != Process::State::NotStarted)
        {
            uint16_t pid = processes[i]->getPid();
            uint8_t priority = processes[i]->getPriority();
            std::string process_state = processStateToString(processes[i]->getState());
            int8_t core = processes[i]->getCpuCore();
            std::string cpu_core = (core >= 0) ? std::to_string(core) : "--";
            double turn_time = processes[i]->getTurnaroundTime();
            double wait_time = processes[i]->getWaitTime();
            double cpu_time = processes[i]->getCpuTime();
            double remain_time = processes[i]->getRemainingTime();
            printf("| %5u | %8u | %10s | %4s | %9.1lf | %9.1lf | %8.1lf | %11.1lf |\n", 
                   pid, priority, process_state.c_str(), cpu_core.c_str(), turn_time, 
                   wait_time, cpu_time, remain_time);
            num_lines++;
        }
    }
    return num_lines;
}

void clearOutput(int num_lines)
{
    int i;
    for (i = 0; i < num_lines; i++)
    {
        fputs("\033[A\033[2K", stdout);
    }
    rewind(stdout);
    fflush(stdout);
}

uint32_t currentTime()
{
    uint32_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now().time_since_epoch()).count();
    return ms;
}

std::string processStateToString(Process::State state)
{
    std::string str;
    switch (state)
    {
        case Process::State::NotStarted:
            str = "not started";
            break;
        case Process::State::Ready:
            str = "ready";
            break;
        case Process::State::Running:
            str = "running";
            break;
        case Process::State::IO:
            str = "i/o";
            break;
        case Process::State::Terminated:
            str = "terminated";
            break;
        default:
            str = "unknown";
            break;
    }
    return str;
}
