#include "StdThreadSafeQueue.hpp"
#include "ThreadSafeQueue.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <atomic>

using namespace std;

using ThreadSafeQueue = childcity::threadsafeqeue::ThreadSafeQueue<string>;
using StdThreadSafeQueue = childcity::threadsafeqeue::StdThreadSafeQueue<string>;

int main()
{
    const int threadCount = 8;
    atomic<bool> startWorking = false;
    thread t[threadCount];

    StdThreadSafeQueue stdSafeQueue;
    ThreadSafeQueue mySafeQueue;
    mutex mainMutex;

    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < threadCount; ++i) {
        t[i] = thread{[&](int i){

            if(i > 3 /*6*/) { // This is 'Writer Thread'
                while (!startWorking.load(memory_order_acquire)); // wait for all treads started

                while (true) {
                    try {
                        //stdSafeQueue.push(to_string(rand())); // NOLINT
                        mySafeQueue.push(to_string(rand())); // NOLINT

                        {// sleep 10 microseconds
                            using namespace std::chrono;
                            auto curr = high_resolution_clock::now();
                            while (duration_cast<microseconds>(high_resolution_clock::now() - curr).count() < 10 /*1800*/);
                        }
                    } catch (exception &ex) {
                        break;
                    }
                }
            } else { // This is 'Reader Thread'
                while (!startWorking.load(memory_order_acquire)); // wait for all treads started

                while (true) {
                    try {
                        lock_guard<mutex> lock(mainMutex);

//                        auto val = stdSafeQueue.tryPop();
//                        auto size = stdSafeQueue.size();
                        auto val = mySafeQueue.tryPop();
                        auto size = 0;//mySafeQueue.size();
                        cout << "pop: " << (val == nullptr ? "Empty" : *val) << " size: " << size << endl;
                    } catch (exception &ex) {
                        break;
                    }
                }
            }

        }, i};
    }

    // enable all threads
    startWorking.store(true, memory_order_release);

    for (auto &i : t) {
        i.join();
    }

    return 0;
}