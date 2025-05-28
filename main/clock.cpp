#include <iostream>
#include <thread>
#include <chrono>


class Clock {
public:
    volatile bool running = true;
    volatile int count = 0;
    std::chrono::milliseconds dura{5};
    //Init Clock speed to to counts per second

    int getClockCount(){
        return count;
    }

    //Main Ticking Loop
    void run() {
        while (running){
            this->tick();
        }
    }

    //Default Constructor
    Clock() : count(0), dura(5), running(true) {}

private:
    void tick() {
        count += 1;
        std::this_thread::sleep_for(dura);
    }
};