#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <atomic>
#include <mutex>
#include <thread>

extern std::mutex coutMutex;

class Task {
public:
    int taskID;
    int taskPriority;
    std::vector<std::string> taskArgs; 
    std::atomic<bool> interrupted{false}; // Make interrupted atomic
    std::atomic<bool> completed{false};   // Make completed atomic


    //Valid Constrcutor
    Task(int id, int priority, std::vector<std::string> name) : taskID(id), taskPriority(priority), taskArgs(name) {}

    //Malformed Task Request
    Task() : taskID(0), taskPriority(0), taskArgs({"MALFORMED"}){
    }
 

    bool checkInterrupt() {
        return interrupted.load(std::memory_order_acquire);
    }

    void interrupt() {
        interrupted.store(true, std::memory_order_release);
    }

    void execute(){    
        if (taskArgs.empty()) {
            std::cout << "No Arguments Provided" << std::endl;
            completed.store(true, std::memory_order_release);
            return;
        }


        switch (taskArgs.size()) {
            case 0:
                std::cout << "No Arguments Provided" << std::endl;
                completed.store(true, std::memory_order_release);
                return;
            case 1:
                std::cout << "Insufficient Arguments Provided" << std::endl;
                completed.store(true, std::memory_order_release);
                return;
            default:
                completed.store(true, std::memory_order_release);
                break;
        }

        std::string command = taskArgs.at(0);

        try{
            if (command == "add") {
                if (taskArgs.size() < 3) {
                    std::cout << "Insufficient Arguments for add" << std::endl;
                    return;
                }
                add(std::stoi(taskArgs.at(1)), std::stoi(taskArgs.at(2)));
            } else if (command == "print") {
                if (taskArgs.size() < 2) {
                    std::cout << "Insufficient Arguments for print" << std::endl;
                    return;
                }
                print(taskArgs.at(1));
            } else if (command == "reverse") {
                reverse(taskArgs.at(1));
            } else if (command == "factorial") {
                factorial(std::stoi(taskArgs.at(1)));
            }else{
                std::cout << "Unknown command: " << command << std::endl;
            }
        }catch (const std::invalid_argument& e) {
            std::cout << "Invalid argument: " << e.what() << std::endl;
        } catch (const std::out_of_range& e) {
            std::cout << "Argument out of range: " << e.what() << std::endl;
        } catch (...) {
            std::cout << "An unexpected error occurred." << std::endl;
        }
        completed.store(true, std::memory_order_release);
    }




private:

    inline void add(int a, int b){
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout<< (a+b) << std::endl;
        completed.store(true, std::memory_order_release); // Ensure completed is set after the output
    }

    void factorial(int n) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate some processing time

        if (checkInterrupt()) return;

        if (n == 0 || n == 1) {
            if (checkInterrupt()) return;
            {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cout << "Factorial of " << n << " is 1" << std::endl;
            }
            completed.store(true, std::memory_order_release);
            return;
        }

        long long result = 1;
        while (n > 1) {
            if (checkInterrupt()) return;
            result *= n;
            n--;
        }

        // FINAL check before output
        if (checkInterrupt()) return;

        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "Factorial is: " << result << std::endl;
        }
        completed.store(true, std::memory_order_release);
    }

    inline void mult (int a, int b){
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout<< (a * b) << std::endl;
        completed.store(true, std::memory_order_release);
    }

    void print (std::string output){
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "Printing " + output << std::endl;
        completed.store(true, std::memory_order_release);
    }

    void reverse(std::string input){

        std::lock_guard<std::mutex> lock(coutMutex);
        std::string output = std::string(input.rbegin(), input.rend());
        std::cout<< output << std::endl;
        std::cout<< std::flush;
        completed.store(true, std::memory_order_release);

    }
};