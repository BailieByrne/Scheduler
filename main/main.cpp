#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <mutex>
#include <algorithm>
#include "clock.cpp"
#include "task.cpp"


std::mutex coutMutex; // Mutex for thread-safe console output

//Scheduler Class
class TaskScheduler {
public:

    //Init public vars ensure count is volatile
    std::vector<std::shared_ptr<Task>> taskList;
    Clock taskClock;
    volatile int lastCount;
    bool running = true;
    int taskID = 0;

    //Simple Method to add tasks
    void addTask(std::shared_ptr<Task> task){
        std::lock_guard<std::mutex> lock(taskListMutex); // Lock the task list for thread safety
        taskList.emplace_back(task);
    }

    //Main running loop to execute tasks
    void run(){
        while (running) {
            if (taskClock.getClockCount() > lastCount) {
            if (taskList.empty()) {
                std::lock_guard<std::mutex> lock(coutMutex); // Lock the task list for thread safety
                std::cout << "IDLE" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(15));
                lastCount = taskClock.getClockCount();
                continue;
                }
            }

            std::lock_guard<std::mutex> lock(taskListMutex); // Lock the task list for thread safety

            // Find the highest priority task
            auto highestPrioTask = std::max_element(taskList.begin(), taskList.end(), [](const std::shared_ptr<Task>& a, const std::shared_ptr<Task>& b){
                return a->taskPriority < b->taskPriority;
            });

            if (highestPrioTask != taskList.end()) {
                Task* taskPtr = highestPrioTask->get();

                taskPtr->taskID = taskID++; // Assign a unique ID to the task

                taskPtr->interrupted.store(false, std::memory_order_release); // Reset the interrupted flag
                taskPtr->completed.store(false, std::memory_order_release); // Reset the completed flag

                //Begin task execution
                std::thread t(&Task::execute, taskPtr);
                t.detach();
                
                // Wait for the task to complete or timeout
                std::thread timer([this, taskPtr, startcount = taskClock.getClockCount(), highestPrioTask]() {
                    while (taskClock.getClockCount() - startcount <  taskPtr->timeLimit){
                        if (taskPtr->completed.load(std::memory_order_acquire)) {
                            taskList.erase(highestPrioTask);
                            return;
                        }
                    }
                    taskPtr->interrupt();
                    taskPtr->taskPriority = std::max(taskPtr->taskPriority - 1, -1); // Decrease priority if not completed
                    taskPtr->timeLimit = std::min(taskPtr->timeLimit + 1, 100); //Clamp timeLimit to a maximum of 500ms
                });
                
                timer.join();

            }

            lastCount = taskClock.getClockCount();
            }
        }
    //Default Construcctor
    TaskScheduler() : taskList({}), taskClock(Clock()), lastCount(0), running(true) {
        std::thread clockThread(&Clock::run, &taskClock);
        clockThread.detach();
    };

private:
    std::mutex taskListMutex; // Mutex for thread safety
};



int main(){
    TaskScheduler systemScheduler;

    //Test Tasks
    auto firsttask = std::make_shared<Task>(1,std::vector<std::string>{"add","5","6"});
    auto secondtask = std::make_shared<Task>(5,std::vector<std::string>{"add","5","5"});
    auto thirdtask = std::make_shared<Task>();
    auto testtask = std::make_shared<Task>(7,std::vector<std::string>{"reverse","TESTING TASKS"});
    auto factorialtask = std::make_shared<Task>(15,std::vector<std::string>{"factorial","5"});
    auto errortask = std::make_shared<Task>();
    auto fourthtask = std::make_shared<Task>(6,std::vector<std::string>{"print","HELLO WORLD"});


    //Adding random tasks to the scheduler
    for (int i = 0; i < 100; ++i) {
        auto randomTask = std::make_shared<Task>(rand() % 15, std::vector<std::string>{"add", std::to_string(rand() % 100), std::to_string(rand() % 100)});
        systemScheduler.addTask(randomTask);
    }

    systemScheduler.addTask(firsttask);
    systemScheduler.addTask(factorialtask);

    //Joing the sechduler thread to block main from ending
    std::thread schedulerThread(&TaskScheduler::run, &systemScheduler);
    schedulerThread.join();

    return 0;
}